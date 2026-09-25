// Mechanism 5 (K5): a CRTP factory that activates a subscriber after its most-derived constructor has
// run, instead of requiring an explicit trySubscribe() call at the end of every derived constructor.
// Built on mechanism 1 (handshake); the activation technique is independent of the quiescence mechanism
// chosen and composes with any of them.
#pragma once
#include "qx_handshake.hpp"
#include <memory>
#include <utility>

namespace qx::crtp {

/// Base for a subscriber that wants automatic activation. Does NOT subscribe in its constructor --
/// activation happens only through make(), after Derived is fully constructed.
template<class Derived, class Data>
class Subscriber : public hs::Subscribe<Data>
{
protected:
    Subscriber() noexcept = default; // does not activate; make() does, after Derived is fully constructed

public:
    /// The only supported way to create Derived: guarantees activation happens post-construction.
    /// Trade-off vs explicit trySubscribe(): Derived must be heap-allocated through this factory (a
    /// stack-allocated Derived can still forget to activate -- CRTP alone cannot hook "end of
    /// most-derived constructor" for automatic storage; only a factory for a single allocation site can).
    template<class... Args>
    static std::unique_ptr<Derived> make(Args&&... args)
    {
        auto p = std::make_unique<Derived>(std::forward<Args>(args)...);
        static_cast<hs::Subscribe<Data>*>(p.get())->activate();
        return p;
    }
};

} // namespace qx::crtp
