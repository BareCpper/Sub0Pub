/** A recording session has two subscriber slots. A one-shot recorder frees its slot for a waiting recorder. */
#include "sub0pub/broker.hpp"

struct TemperatureReading
{
    int celsius;
    using sub0_config = sub0::config<sub0::Scoped, sub0::Capacity<2>, sub0::Snapshot>;
};

struct TemperatureSensor final : sub0::Publish<TemperatureReading>
{
    using Publish::Publish;

    void measure(int celsius) noexcept { sub0::publish(*this, TemperatureReading{celsius}); }
};

struct TemperatureRecorder final : sub0::Subscribe<TemperatureReading>
{
    using Subscribe::Subscribe;
    unsigned readingsReceived = 0;
    int lastCelsius = 0;

    void receive(const TemperatureReading& reading) noexcept override
    {
        lastCelsius = reading.celsius;
        ++readingsReceived;
    }
};

struct FirstReadingRecorder final : sub0::Subscribe<TemperatureReading>
{
    using Subscribe::Subscribe;
    unsigned readingsReceived = 0;
    int lastCelsius = 0;

    void receive(const TemperatureReading& reading) noexcept override
    {
        lastCelsius = reading.celsius;
        ++readingsReceived;
        disconnect(); // Snapshot lets a callback remove itself without skipping the next receiver.
    }
};

int main()
{
    // The session owns the subscription table and must outlive every handle bound to it.
    sub0::Domain<TemperatureReading> session;
    TemperatureSensor thermometer(session);

    // Unlocked subscribers register during construction, in this order.
    FirstReadingRecorder firstReading(session);
    TemperatureRecorder continuousRecorder(session);
    TemperatureRecorder waitingRecorder(session);
    if (waitingRecorder.trySubscribe() != sub0::SubscribeResult::CapacityExceeded)
        return 1;

    thermometer.measure(20);
    if (firstReading.readingsReceived != 1 || continuousRecorder.readingsReceived != 1)
        return 2;

    // The one-shot recorder is still alive, but has released its slot.
    if (firstReading.isSubscribed() || waitingRecorder.trySubscribe() != sub0::SubscribeResult::Subscribed)
        return 3;

    thermometer.measure(21);
    if (firstReading.readingsReceived != 1 || continuousRecorder.readingsReceived != 2 ||
        waitingRecorder.readingsReceived != 1)
        return 4;

    session.close(); // Detaches subscribers; existing handles stay alive but cannot resume the session.
    thermometer.measure(22);
    if (waitingRecorder.trySubscribe() != sub0::SubscribeResult::Closed)
        return 5;

    return continuousRecorder.readingsReceived == 2 && waitingRecorder.readingsReceived == 1 &&
        firstReading.lastCelsius == 20 && continuousRecorder.lastCelsius == 21 && waitingRecorder.lastCelsius == 21 ? 0 : 6;
}
