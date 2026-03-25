/** Sub0Pub Example: Multi-Type Subscribe
 *
 * Demonstrates:
 *   - Publishing multiple types from one class
 *   - Subscribing to multiple types with SubscribeAll
 *   - Type-safe routing — each type goes to its own receive() override
 */
#include "sub0pub/sub0pub.hpp"
#include <cstdio>

// A system that publishes both sensor data and status codes
struct SensorData { float temperature; float humidity; };
struct StatusCode { int code; };

class SensorHub : public sub0::Publish<SensorData>
                , public sub0::Publish<StatusCode>
{
public:
    void reportData(float temp, float hum) {
        sub0::publish(this, SensorData{temp, hum});
    }
    void reportStatus(int code) {
        sub0::publish(this, StatusCode{code});
    }
};

// A monitor that receives both types
class Monitor : public sub0::SubscribeAll<SensorData, StatusCode> {
public:
    void receive(const SensorData& data) noexcept override {
        std::printf("  [Monitor] Sensor: temp=%.1f, humidity=%.1f\n",
                    data.temperature, data.humidity);
    }
    void receive(const StatusCode& status) noexcept override {
        std::printf("  [Monitor] Status: code=%d\n", status.code);
    }
};

// A logger that only cares about status codes
class StatusLogger : public sub0::Subscribe<StatusCode> {
public:
    void receive(const StatusCode& status) noexcept override {
        std::printf("  [StatusLogger] Logged status code: %d\n", status.code);
    }
};

int main()
{
    std::printf("=== Multi-Type Subscribe ===\n");

    SensorHub hub;
    Monitor monitor;        // Receives both SensorData and StatusCode
    StatusLogger logger;    // Receives only StatusCode

    std::printf("Reporting sensor data:\n");
    hub.reportData(22.5f, 45.0f);

    std::printf("\nReporting status:\n");
    hub.reportStatus(200);

    std::printf("\nReporting more data (logger doesn't see this):\n");
    hub.reportData(23.0f, 44.5f);

    std::printf("\nSubscribeAll::Count = %zu\n", Monitor::Count);

    return 0;
}
