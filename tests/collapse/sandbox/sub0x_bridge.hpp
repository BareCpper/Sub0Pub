#pragma once
/** PROTOTYPE (issue #9 spike): joining a static wiring (sub0x_static.hpp, pattern B) to a runtime registry
 * for genuinely dynamic subscribers (plugins, diagnostics, late subscribers), without paying for the
 * dynamic side when it is unused, and without making the always-known static receivers pay for it either.
 *
 * Three alternatives, all bindable at the composition point like any other receiver:
 *
 *   A  BrokerPort<T>     bound as one entry of a static Wiring/StaticWiring; forwards to a full #8 runtime
 *                        registry (sub0x_broker.hpp: Domain/Subscribe/Publish -- policy, filter, disconnect
 *                        contract, thread-safety all apply to the dynamic side only).
 *   B  DynamicPort<T,N>  bound the same way; owns a minimal fixed-capacity slot array itself (no broker
 *                        dependency, no policy) -- the least a dynamic side can cost.
 *   C  StaticAdapter<Bus,T>  the inverse direction: registers the *static* wiring as one subscriber of a
 *                        runtime registry, so a dynamic publisher reaches static receivers. Included for
 *                        comparison: routing static receivers through the registry's virtual dispatch to
 *                        share one call site with dynamic ones costs the static side something, which A/B
 *                        do not.
 *
 * A and B answer "static publisher -> dynamic subscribers" (the required scenario); C demonstrates the
 * "dynamic publisher -> static receivers" direction and what it costs to fold the two into one dispatch.
 */
#include "sub0x_broker.hpp" // #8 runtime-registry prototype (Domain, Subscribe, Publish, config)

#include <cstdint>

namespace sub0x
{
    /** A: bridge to a full #8 runtime registry. Bind like any receiver: `StaticWiring<&a, &b, &port>`.
     * `receive(const T&)` makes it visible to detail::accepts, so the static wiring's capability-based
     * routing calls it in bound position; behind it, dispatch is exactly Publish<T>::publish(). */
    template<class T>
    class BrokerPort : public sub0x::Publish<T>
    {
    public:
        template<class C = sub0x::config_t<T>, std::enable_if_t<C::storage == sub0x::Storage::Global, int> = 0>
        BrokerPort() noexcept {}

        template<class C = sub0x::config_t<T>, std::enable_if_t<C::storage == sub0x::Storage::Scoped, int> = 0>
        explicit BrokerPort(sub0x::Domain<T>& domain) noexcept : sub0x::Publish<T>(domain) {}

        void receive(const T& msg) noexcept { this->publish(msg); }
    };

    /** B: a lightweight intrusive slot array owned directly by the bridge element. No broker, no policy
     * (no filter, no publish context, no thread safety, no capacity beyond N) -- just enough to hold
     * receivers that come and go at runtime. Order of delivery is add() order. */
    template<class T, uint32_t N = 8>
    class DynamicPort
    {
    public:
        struct Receiver
        {
            virtual void receive(const T&) noexcept = 0;
        protected:
            ~Receiver() = default;
        };

        void add(Receiver* r) noexcept { (void)tryAdd(r); }

        /// Same as add(), but reports whether it fit (capacity exceeded is otherwise silent)
        bool tryAdd(Receiver* r) noexcept
        {
            if (count_ >= N)
                return false;
            entries_[count_++] = r;
            return true;
        }

        void remove(Receiver* r) noexcept
        {
            for (uint32_t i = 0; i < count_; ++i)
                if (entries_[i] == r)
                {
                    for (uint32_t j = i + 1; j < count_; ++j)
                        entries_[j - 1] = entries_[j];
                    --count_;
                    return;
                }
        }

        void receive(const T& msg) const noexcept { for (uint32_t i = 0; i < count_; ++i) entries_[i]->receive(msg); }

    private:
        Receiver* entries_[N] = {};
        uint32_t count_ = 0;
    };

    /** C: the inverse bridge -- registers a static wiring (Wiring or StaticWiring) as ONE subscriber of a
     * #8 runtime registry, so a dynamic-side publish() reaches every static receiver too. `Bus` must expose
     * `static`/instance `publish(const T&)`; only the static-member form (StaticWiring) is used here. */
    template<class Bus, class T>
    class StaticAdapter final : public sub0x::Subscribe<T>
    {
    public:
        template<class C = sub0x::config_t<T>, std::enable_if_t<C::storage == sub0x::Storage::Global, int> = 0>
        StaticAdapter() noexcept {}

        template<class C = sub0x::config_t<T>, std::enable_if_t<C::storage == sub0x::Storage::Scoped, int> = 0>
        explicit StaticAdapter(sub0x::Domain<T>& domain) noexcept : sub0x::Subscribe<T>(domain) {}

        void receive(const T& msg) noexcept override { Bus::publish(msg); }
    };
}
