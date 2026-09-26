#pragma once
/** C++20 fork of sub0x_static.hpp (issue #9 spike: docs/design/spikes/cxx23_upgrade.md, face-off 2).
 *
 * Same public surface as tests/collapse/sandbox/sub0x_static.hpp (Wiring, StaticWiring) for the subset those
 * two collapse cases need; only the detection idioms are rewritten as C++20 concepts instead of the SFINAE
 * void_t traits (detail::accepts, detail::has_filter, detail::has_get). Kept as a separate file, additive
 * only, so the C++17 sandbox header and its variants stay untouched.
 */
#include <tuple>
#include <type_traits>
#include <utility>
#include <concepts>

namespace sub0x20
{
    namespace detail
    {
        /// A receiver accepts T if it has receive(const T&)
        template<class R, class T>
        concept Receiver = requires(R& r, const T& t) { r.receive(t); };

        /// A receiver filters T if it has filter(const T&) convertible to bool
        template<class R, class T>
        concept FilteringReceiver = requires(R& r, const T& t) {
            { r.filter(t) } -> std::convertible_to<bool>;
        };

        /// Bound objects may be the receiver itself or a holder exposing it via get()
        template<class X>
        concept HasGet = requires(X& x) { x.get(); };

        template<class X>
        constexpr decltype(auto) receiver(X& x) noexcept
        {
            if constexpr (HasGet<X>)
                return x.get();
            else
                return (x);
        }

        /// Deliver to one receiver: nothing at all if it does not handle T; its filter only if it declares one
        template<class R, class T>
        inline void deliver(R& r, const T& msg) noexcept
        {
            if constexpr (Receiver<R, T>)
            {
                if constexpr (FilteringReceiver<R, T>)
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

        template<class T>
        void publish(const T& msg) const noexcept
        {
            std::apply([&](auto&... b) { (detail::deliver(detail::receiver(b), msg), ...); }, bound_);
        }

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
}
