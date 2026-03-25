/** Sub0Pub Example: Message Filtering
 *
 * Demonstrates:
 *   - Override filter() to selectively receive messages
 *   - Filter runs before receive() — rejected messages cost almost nothing
 *   - Multiple subscribers with different filters on the same type
 */
#include "sub0pub/sub0pub.hpp"
#include <cstdio>

struct LogEntry {
    int level;      // 0=debug, 1=info, 2=warn, 3=error
    const char* msg;
};

class Logger : public sub0::Publish<LogEntry> {
public:
    void log(int level, const char* msg) {
        sub0::publish(this, LogEntry{level, msg});
    }
};

// Only receives warnings and errors (level >= 2)
class AlertDisplay : public sub0::Subscribe<LogEntry> {
public:
    bool filter(const LogEntry& entry) noexcept override {
        return entry.level >= 2;
    }
    void receive(const LogEntry& entry) noexcept override {
        std::printf("  [ALERT] level=%d: %s\n", entry.level, entry.msg);
    }
};

// Receives everything
class FileLog : public sub0::Subscribe<LogEntry> {
public:
    void receive(const LogEntry& entry) noexcept override {
        std::printf("  [FILE]  level=%d: %s\n", entry.level, entry.msg);
    }
};

// Only receives debug messages
class DebugConsole : public sub0::Subscribe<LogEntry> {
public:
    bool filter(const LogEntry& entry) noexcept override {
        return entry.level == 0;
    }
    void receive(const LogEntry& entry) noexcept override {
        std::printf("  [DEBUG] %s\n", entry.msg);
    }
};

int main()
{
    std::printf("=== Message Filtering ===\n\n");

    Logger logger;
    AlertDisplay alerts;   // level >= 2 only
    FileLog file;          // everything
    DebugConsole debug;    // level == 0 only

    logger.log(0, "System starting up");
    logger.log(1, "Connected to server");
    logger.log(2, "Disk space low");
    logger.log(3, "Connection lost!");
    logger.log(0, "Retrying...");

    return 0;
}
