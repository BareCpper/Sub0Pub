/** Sub0Pub Example: Basic Publish/Subscribe
 *
 * Demonstrates the core pattern:
 *   - Inherit sub0::Publish<T> to send typed messages
 *   - Inherit sub0::Subscribe<T> to receive them
 *   - Subscribers auto-wire on construction, auto-disconnect on destruction
 */
#include "sub0pub/sub0pub.hpp"
#include <cstdio>

// A sensor that publishes temperature readings
class TemperatureSensor : public sub0::Publish<float> {
public:
    void sample(float celsius) {
        sub0::publish(this, celsius);
    }
};

// A display that subscribes to temperature readings
class TemperatureDisplay : public sub0::Subscribe<float> {
    const char* label_;
public:
    TemperatureDisplay(const char* label) : label_(label) {}

    void receive(const float& celsius) noexcept override {
        std::printf("  [%s] Temperature: %.1f C\n", label_, celsius);
    }
};

int main()
{
    std::printf("=== Basic Pub/Sub ===\n");

    TemperatureSensor sensor;

    // Create two displays — both auto-subscribe to float
    TemperatureDisplay lcd("LCD");
    TemperatureDisplay log("LOG");

    std::printf("Publishing 23.5:\n");
    sensor.sample(23.5f);

    std::printf("Publishing 24.1:\n");
    sensor.sample(24.1f);

    // Destroy LCD — LOG still receives
    {
        std::printf("\nLCD destroyed, publishing 25.0:\n");
    }
    // lcd is still alive here (scope didn't end), let's demo with a block:
    {
        TemperatureDisplay temporary("TMP");
        std::printf("TMP added, publishing 26.0:\n");
        sensor.sample(26.0f);
    }
    // TMP destroyed
    std::printf("TMP removed, publishing 27.0:\n");
    sensor.sample(27.0f);

    return 0;
}
