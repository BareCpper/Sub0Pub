// Compile-only workload shared byte-for-byte by both revisions. Each translation unit instantiates the same
// public API with distinct message types, as independently compiled consumer components do. No link is timed.
// PROFILE: 0 = narrow wiring, 1 = narrow broker, 2 = umbrella with the same wiring work as profile 0.
#if PROFILE == 0
#include <sub0pub/wiring.hpp>
#elif PROFILE == 1
#include <sub0pub/broker.hpp>
#else
#include <sub0pub/sub0pub.hpp>
#endif

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>

namespace {

template<std::size_t Type, std::size_t Unit = TRANSLATION_UNIT>
struct Message { std::uint32_t value; };

#if PROFILE == 1
template<std::size_t Type, std::size_t Receiver>
struct Listener final : sub0::Subscribe<Message<Type>>
{
    std::uint32_t total = 0;
    void receive(const Message<Type>& message) noexcept override { total += message.value ^ Receiver; }
};

template<std::size_t Type>
struct Source final : sub0::Publish<Message<Type>> {};

template<std::size_t Type, std::size_t... Receiver>
std::uint32_t dispatch(std::uint32_t value, std::index_sequence<Receiver...>)
{
    std::tuple<Listener<Type, Receiver>...> listeners;
    Source<Type> source;
    sub0::publish(source, Message<Type>{value});
    return (std::get<Receiver>(listeners).total + ... + 0U);
}

template<std::size_t... Type>
std::uint32_t consume(std::uint32_t value, std::index_sequence<Type...>)
{
    return (dispatch<Type>(value, std::make_index_sequence<RECEIVER_COUNT>{}) + ... + 0U);
}
#else
template<std::size_t Receiver>
struct Listener
{
    std::uint32_t total = 0;
    template<std::size_t Type>
    void receive(const Message<Type>& message) noexcept { total += message.value ^ (Type + Receiver); }
};

template<class Wiring, std::size_t... Type>
void dispatch(Wiring& wiring, std::uint32_t value, std::index_sequence<Type...>)
{
    (wiring.publish(Message<Type>{value}), ...);
    const sub0::Sink<Message<0>> output(wiring);
    const auto copied = output;
    copied.publish(Message<0>{value});
}

template<std::size_t... Receiver>
std::uint32_t consume(std::uint32_t value, std::index_sequence<Receiver...>)
{
    std::tuple<Listener<Receiver>...> listeners;
    auto wiring = sub0::wire(std::get<Receiver>(listeners)...);
    dispatch(wiring, value, std::make_index_sequence<MESSAGE_TYPES>{});
    return (std::get<Receiver>(listeners).total + ... + 0U);
}
#endif

} // namespace

#define SUB0PUB_BENCH_JOIN_INNER(a, b) a##b
#define SUB0PUB_BENCH_JOIN(a, b) SUB0PUB_BENCH_JOIN_INNER(a, b)
std::uint32_t SUB0PUB_BENCH_JOIN(compileWorkload, TRANSLATION_UNIT)(std::uint32_t value)
{
#if PROFILE == 1
    return consume(value, std::make_index_sequence<MESSAGE_TYPES>{});
#else
    return consume(value, std::make_index_sequence<RECEIVER_COUNT>{});
#endif
}
