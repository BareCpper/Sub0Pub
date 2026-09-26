#pragma once
/** PROTOTYPE (issue #9, Phase 1 sandbox): typed bindings at the application's composition point ("pattern B")
 *
 * Receivers are ordinary classes: a non-virtual `receive(const T&)` per message type they handle, and optionally
 * `bool filter(const T&)`. No base class, no registry, no registration at construction.
 *
 * The application binds concrete receiver *instances* where it composes itself; their concrete types are kept all
 * the way to the call, so each delivery is a direct (inlinable) call:
 *
 *   B1  auto bus = sub0x::wire(controllerA, controllerB, logger);    // runtime addresses, static types
 *       template<class Out> struct Sensor { Out& out; ... out.publish(Sample{v}); };
 *   B2  using Bus = sub0x::StaticWiring<&controllerA, &controllerB, &logger>;   // static storage: no RAM, fixed targets
 *   B3  struct Sensor { sub0x::Sink<Sample> out; ... };   // non-template publisher: one indirect call into typed wiring
 *
 * Routing is by capability: publish<T>() calls, in bound order, every bound receiver that has receive(const T&).
 * Message definitions never list receivers (per-message policy and application wiring stay separate).
 * Two instances of the same receiver type are two bindings; two independent sessions are two wirings.
 */
#include <cassert>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#if defined(__has_include)
#if __has_include(<expected>)
#include <expected> // only used by the optional C++23 Alt 1c below (__cpp_lib_expected); inert under C++17
#endif
#endif

namespace sub0x
{
    namespace detail
    {
        template<class R, class T, class = void> struct accepts : std::false_type {};
        template<class R, class T>
        struct accepts<R, T, std::void_t<decltype(std::declval<R&>().receive(std::declval<const T&>()))>> : std::true_type {};

        template<class R, class T, class = void> struct has_filter : std::false_type {};
        template<class R, class T>
        struct has_filter<R, T, std::void_t<decltype(bool(std::declval<R&>().filter(std::declval<const T&>())))>> : std::true_type {};

        /// Bound objects may be the receiver itself or a holder exposing it via get() (e.g. application storage slots)
        template<class X, class = void> struct has_get : std::false_type {};
        template<class X> struct has_get<X, std::void_t<decltype(std::declval<X&>().get())>> : std::true_type {};

        template<class X>
        constexpr decltype(auto) receiver(X& x) noexcept
        {
            if constexpr (has_get<X>::value)
                return x.get();
            else
                return (x);
        }

        /// Deliver to one receiver: nothing at all if it does not handle T; its filter only if it declares one
        template<class R, class T>
        inline void deliver(R& r, const T& msg) noexcept
        {
            if constexpr (accepts<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return;
                r.receive(msg);
            }
        }

        /// A binding adapter that only refers to the real endpoint (e.g. Forward<Transport>) declares
        /// `using sub0x_by_value = void;` so a Wiring holds it by value: one hop to the endpoint, as hand-written
        template<class B, class = void> struct by_value : std::false_type {};
        template<class B> struct by_value<B, std::void_t<typename B::sub0x_by_value>> : std::true_type {};
        template<class B> using stored_t = std::conditional_t<by_value<B>::value, B, B&>;

        /// Endpoint identity for split horizon: the bound object, or for a by-value adapter what it refers to
        template<class X, class = void> struct has_identity : std::false_type {};
        template<class X> struct has_identity<X, std::void_t<decltype(std::declval<const X&>().sub0x_identity())>> : std::true_type {};
        template<class X>
        constexpr const void* identity(const X& x) noexcept
        {
            if constexpr (has_identity<X>::value)
                return x.sub0x_identity();
            else
                return static_cast<const void*>(&x);
        }

        template<class X> using receiver_t = std::remove_cv_t<std::remove_reference_t<decltype(receiver(std::declval<X&>()))>>;
        template<class Origin, class... R>
        constexpr std::size_t countOf = (std::size_t(std::is_same_v<std::remove_cv_t<Origin>, R>) + ... + std::size_t(0));

        /// Split horizon: do not send a message back to the binding it came from. When the origin's type is bound
        /// exactly once (OriginUnique), the origin *is* that binding: decided at compile time, no address compare
        /// (precondition: the origin is one of the bound endpoints, asserted in debug builds).
        template<bool OriginUnique, class R, class T, class Origin>
        inline void deliverExcept(R& r, const T& msg, const Origin& origin) noexcept
        {
            if constexpr (std::is_same_v<std::remove_cv_t<R>, std::remove_cv_t<Origin>>)
            {
                if constexpr (OriginUnique)
                {
                    assert(identity(r) == identity(origin) && "publishFrom: origin is not a bound endpoint");
                    return;
                }
                else if (identity(r) == identity(origin))
                    return;
            }
            deliver(r, msg);
        }

        /// Origin identified by type alone (publishFrom<Origin>(msg)): the one binding of that type is skipped
        template<class Origin, class R, class T>
        inline void deliverExceptType(R& r, const T& msg) noexcept
        {
            if constexpr (!std::is_same_v<std::remove_cv_t<R>, std::remove_cv_t<Origin>>)
                deliver(r, msg);
        }
    }

    /** B1: receivers bound by reference at the composition point (runtime addresses, static types) */
    template<class... Bound>
    class Wiring
    {
    public:
        constexpr explicit Wiring(Bound&... bound) noexcept : bound_(bound...) {}

        /// Deliver to every bound receiver that handles T, in bound order
        template<class T>
        void publish(const T& msg) const noexcept { publish(msg, Indices{}); }

        /// Ingress from one of the bound receivers (e.g. a transport endpoint): every other receiver gets it
        template<class T, class Origin>
        void publishFrom(const Origin& origin, const T& msg) const noexcept
        {
            publishFrom(origin, msg, Indices{});
        }

        /// Ingress identified by the origin's type, which must be bound exactly once: no origin object needed
        template<class Origin, class T>
        void publishFrom(const T& msg) const noexcept
        {
            static_assert(detail::countOf<Origin, detail::receiver_t<Bound>...> == 1,
                          "publishFrom<Origin>: Origin must be bound exactly once; pass the origin object instead");
            publishFromType<Origin>(msg, Indices{});
        }

    private:
        // Each binding is read (std::get) right before its own delivery, as hand-written code does. std::apply
        // would read every binding up front and keep them live across the calls (extra saved registers).
        using Indices = std::index_sequence_for<Bound...>;

        template<class T, std::size_t... I>
        void publish(const T& msg, std::index_sequence<I...>) const noexcept
        {
            (detail::deliver(detail::receiver(std::get<I>(bound_)), msg), ...);
        }

        template<class T, class Origin, std::size_t... I>
        void publishFrom(const Origin& origin, const T& msg, std::index_sequence<I...>) const noexcept
        {
            constexpr bool unique = detail::countOf<Origin, detail::receiver_t<Bound>...> == 1;
            (detail::deliverExcept<unique>(detail::receiver(std::get<I>(bound_)), msg, origin), ...);
        }

        template<class Origin, class T, std::size_t... I>
        void publishFromType(const T& msg, std::index_sequence<I...>) const noexcept
        {
            (detail::deliverExceptType<Origin>(detail::receiver(std::get<I>(bound_)), msg), ...);
        }

        // mutable: publish() is const, and a by-value adapter must stay callable through it (a const adapter
        // whose receive() is non-const would silently stop matching the capability check)
        mutable std::tuple<detail::stored_t<Bound>...> bound_;
    };

    template<class... Bound>
    constexpr Wiring<Bound...> wire(Bound&... bound) noexcept { return Wiring<Bound...>(bound...); }

    /** Static-path cancellation (issue #9 spike, docs/design/spikes/static_cancellation.md): a receiver-
     * controlled early stop of the *current* publication, so later bound receivers (in bound order) are not
     * delivered to. Four alternatives, all additive: none of them changes the existing `deliver`/`publish`
     * used by the non-cancelling cases (zero_receivers, one_receiver, multi_receivers, filters, ...), so those
     * keep byte-identical codegen. Each alternative below is opt-in per publish call, not per wiring. */
    class Delivery; // forward declaration: used by Alt 2's SFINAE detection below, defined after it

    namespace detail
    {
        /// Alt 1 (bool return): a receiver may return bool from receive() instead of void; false stops the
        /// rest of this publication. Detected at compile time; a void receiver is unaffected.
        template<class R, class T>
        using receive_result_t = decltype(std::declval<R&>().receive(std::declval<const T&>()));

        template<class R, class T, class = void> struct receive_returns_bool : std::false_type {};
        template<class R, class T>
        struct receive_returns_bool<R, T, std::enable_if_t<std::is_same_v<receive_result_t<R, T>, bool>>> : std::true_type {};

        /// Deliver to one receiver; returns whether the publication should continue (true) or stop (false).
        /// A receiver that does not handle T, or is filtered out, or returns void, always continues.
        template<class R, class T>
        inline bool deliverContinue(R& r, const T& msg) noexcept
        {
            if constexpr (accepts<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return true;
                if constexpr (receive_returns_bool<R, T>::value)
                    return r.receive(msg);
                else
                {
                    r.receive(msg);
                    return true;
                }
            }
            else
                return true;
        }

        /// Alt 2 (cancellation token): a receiver may optionally accept a second `sub0x::Delivery&` parameter
        /// and call `delivery.stop()`. Receivers that take only `receive(const T&)` are unaffected.
        template<class R, class T, class = void> struct accepts_token : std::false_type {};
        template<class R, class T>
        struct accepts_token<R, T, std::void_t<decltype(std::declval<R&>().receive(std::declval<const T&>(), std::declval<Delivery&>()))>> : std::true_type {};
    }

    /// Alt 2: passed by reference to every receiver bound to a `publishWithToken` call. A receiver that never
    /// asks for it never sees it; one is constructed per publication (stack-local, not shared state).
    class Delivery
    {
    public:
        void stop() noexcept { stop_ = true; }
        bool stopped() const noexcept { return stop_; }
    private:
        bool stop_ = false;
    };

    namespace detail
    {
        template<class R, class T>
        inline bool deliverToken(R& r, const T& msg, Delivery& delivery) noexcept
        {
            if constexpr (accepts_token<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return true;
                r.receive(msg, delivery);
                return !delivery.stopped();
            }
            else if constexpr (accepts<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return true;
                r.receive(msg);
                return true;
            }
            else
                return true;
        }

        /// Alt 3 (thread-local publish context): mirrors the runtime path's Broker::cancel() (sub0pub.hpp).
        /// A receiver calls the free function sub0x::cancel() from inside receive(); every bound receiver
        /// checks it after being delivered to. Save/restore around each publication makes nested publications
        /// (including of the same message type, e.g. two independent wirings) independent, exactly like the
        /// runtime path — but, like the runtime path, it costs a TLS access even for receivers that never cancel.
        // SUB0X_STATIC_CANCEL_CONTEXT: single-threaded images keep the flag in plain static storage (no TLS);
        // the counterpart of #8's StaticContext. Default: thread_local, correct with concurrent publishers.
#if defined(SUB0X_STATIC_CANCEL_CONTEXT) && SUB0X_STATIC_CANCEL_CONTEXT
        inline bool g_canceled = false;
#else
        inline thread_local bool g_canceled = false;
#endif

        template<class R, class T>
        inline bool deliverTLS(R& r, const T& msg) noexcept
        {
            deliver(r, msg);
            return !g_canceled;
        }
    }

    /// Alt 3: call from inside a receive() bound to a `publishCancelableTLS` call to stop the rest of that
    /// publication. Undefined outside of such a call, exactly like sub0::Publish<Data>::cancel().
    inline void cancel() noexcept { detail::g_canceled = true; }

#if defined(__cpp_lib_expected)
    /** C++23 variant of Alt 1 (docs/design/spikes/static_cancellation.md): receive() may return
     * std::expected<void, Stop> instead of a bare bool — a documented reason for stopping, still returned
     * by value (no out-parameter), still detected at compile time and short-circuited by a fold, same as
     * Alt 1. Entirely inert unless the toolchain has <expected> (i.e. -std=c++23); it changes nothing for
     * C++17 builds, which never see this block. Not wired into the collapse ctest harness (that harness is
     * built at C++17 project-wide); see the spike doc for how it was measured. */
    enum class Stop { Canceled };

    namespace detail
    {
        template<class R, class T, class = void> struct receive_returns_expected : std::false_type {};
        template<class R, class T>
        struct receive_returns_expected<R, T, std::enable_if_t<std::is_same_v<receive_result_t<R, T>, std::expected<void, Stop>>>> : std::true_type {};

        template<class R, class T>
        inline bool deliverExpected(R& r, const T& msg) noexcept
        {
            if constexpr (accepts<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return true;
                if constexpr (receive_returns_expected<R, T>::value)
                    return r.receive(msg).has_value();
                else
                {
                    r.receive(msg);
                    return true;
                }
            }
            else
                return true;
        }
    }
#endif

    /** B2: static topology for objects with static storage duration: the targets are template arguments */
    template<auto*... Bound>
    struct StaticWiring
    {
        template<class T>
        static void publish(const T& msg) noexcept
        {
            (detail::deliver(detail::receiver(*Bound), msg), ...);
        }

        template<class T, class Origin>
        static void publishFrom(const Origin& origin, const T& msg) noexcept
        {
            constexpr bool unique = detail::countOf<Origin, detail::receiver_t<std::remove_pointer_t<decltype(Bound)>>...> == 1;
            (detail::deliverExcept<unique>(detail::receiver(*Bound), msg, origin), ...);
        }

        /// Ingress identified by the origin's type, which must be bound exactly once
        template<class Origin, class T>
        static void publishFrom(const T& msg) noexcept
        {
            static_assert(detail::countOf<Origin, detail::receiver_t<std::remove_pointer_t<decltype(Bound)>>...> == 1,
                          "publishFrom<Origin>: Origin must be bound exactly once; pass the origin object instead");
            (detail::deliverExceptType<Origin>(detail::receiver(*Bound), msg), ...);
        }

        /// Alt 1 (bool return): stops delivering to later bound receivers (bound order) as soon as one
        /// receive() returns false. A receiver with a void receive() always continues.
        template<class T>
        static void publishCancelable(const T& msg) noexcept
        {
            (detail::deliverContinue(detail::receiver(*Bound), msg) && ...);
        }

        /// Alt 2 (cancellation token): a fresh sub0x::Delivery per publication; stops delivering to later
        /// bound receivers once any receiver that opted into `receive(const T&, sub0x::Delivery&)` calls stop().
        template<class T>
        static void publishWithToken(const T& msg) noexcept
        {
            Delivery delivery;
            (detail::deliverToken(detail::receiver(*Bound), msg, delivery) && ...);
        }

        /// Alt 3 (thread-local publish context): a receiver calls sub0x::cancel() from inside receive();
        /// every later bound receiver is skipped. Save/restore makes nested publications independent.
        template<class T>
        static void publishCancelableTLS(const T& msg) noexcept
        {
            const bool previous = detail::g_canceled;
            detail::g_canceled = false;
            (detail::deliverTLS(detail::receiver(*Bound), msg) && ...);
            detail::g_canceled = previous;
        }

#if defined(__cpp_lib_expected)
        /// C++23 Alt 1c: as publishCancelable, but a receiver returns std::expected<void, Stop> instead of bool.
        template<class T>
        static void publishCancelableExpected(const T& msg) noexcept
        {
            (detail::deliverExpected(detail::receiver(*Bound), msg) && ...);
        }
#endif
    };

    /** B3: a type-erased publication port for one message type, for publishers that are not templates.
     * One indirect call reaches the typed wiring; everything behind it is still resolved statically. */
    template<class T>
    class Sink
    {
    public:
        /// Wrap a wiring (constrained: copying a Sink must copy it, never wrap it)
        template<class W, std::enable_if_t<!std::is_same_v<std::remove_cv_t<W>, Sink>, int> = 0>
        explicit Sink(W& wiring) noexcept
            : target_(&wiring)
            , call_([](const void* w, const T& msg) noexcept { static_cast<const W*>(w)->publish(msg); })
        {}

        void publish(const T& msg) const noexcept { call_(target_, msg); }

    private:
        const void* target_;
        void (*call_)(const void*, const T&) noexcept;
    };

    /** CRTP publisher mixin (Phase 2 spike, issue #9 publisher-ergonomics face-off): the derived publisher
     * stores a reference to its output and gets `this->publish(msg)` instead of `out.publish(msg)`. Still a
     * template over Out (the mixin argument), so it has the same one-instantiation-per-topology shape as
     * writing `template<class Out>` by hand; only the spelling at the call site changes. */
    template<class Derived, class Out>
    class Publisher
    {
    public:
        constexpr explicit Publisher(const Out& out) noexcept : out_(out) {}

    protected:
        template<class T>
        void publish(const T& msg) const noexcept { out_.publish(msg); }

    private:
        Out out_; // by value: a Wiring is a tuple of receiver references (one hop), a StaticWiring is empty
    };

    /** Typed transport endpoint binding for a transport with static storage: no RAM, fixed target */
    template<auto* TransportObject>
    struct StaticForward
    {
        template<class T>
        auto receive(const T& msg) noexcept -> decltype(detail::receiver(*TransportObject).send(msg), void())
        {
            detail::receiver(*TransportObject).send(msg);
        }
    };

    /** Typed transport endpoint binding: forwards every message type the transport can send.
     * Transport concept: send(const T&) for each message type it carries (result handling is the transport's). */
    template<class Transport>
    class Forward
    {
    public:
        using sub0x_by_value = void; // refers to the transport only: a Wiring holds it by value (one hop)

        constexpr explicit Forward(Transport& transport) noexcept : transport_(transport) {}

        /// Split-horizon identity: the transport it forwards to
        constexpr const void* sub0x_identity() const noexcept { return &transport_; }

        template<class T>
        auto receive(const T& msg) const noexcept -> decltype(std::declval<Transport&>().send(msg), void())
        {
            transport_.send(msg);
        }

    private:
        Transport& transport_;
    };
}
