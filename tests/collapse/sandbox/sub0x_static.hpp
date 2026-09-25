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
#include <tuple>
#include <type_traits>
#include <utility>

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

        template<class R, class T, class Origin>
        inline void deliverExcept(R& r, const T& msg, const Origin& origin) noexcept
        {
            if constexpr (std::is_same_v<std::remove_cv_t<R>, std::remove_cv_t<Origin>>)
                if (static_cast<const void*>(&r) == static_cast<const void*>(&origin))
                    return; // split horizon: do not send a message back to the binding it came from
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
        void publish(const T& msg) const noexcept
        {
            std::apply([&](auto&... b) { (detail::deliver(detail::receiver(b), msg), ...); }, bound_);
        }

        /// Ingress from one of the bound receivers (e.g. a transport endpoint): every other receiver gets it
        template<class T, class Origin>
        void publishFrom(const Origin& origin, const T& msg) const noexcept
        {
            std::apply([&](auto&... b) { (detail::deliverExcept(detail::receiver(b), msg, origin), ...); }, bound_);
        }

    private:
        std::tuple<Bound&...> bound_;
    };

    template<class... Bound>
    constexpr Wiring<Bound...> wire(Bound&... bound) noexcept { return Wiring<Bound...>(bound...); }

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
            (detail::deliverExcept(detail::receiver(*Bound), msg, origin), ...);
        }
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
        const Out& out_;
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
        constexpr explicit Forward(Transport& transport) noexcept : transport_(transport) {}

        template<class T>
        auto receive(const T& msg) noexcept -> decltype(std::declval<Transport&>().send(msg), void())
        {
            transport_.send(msg);
        }

    private:
        Transport& transport_;
    };
}
