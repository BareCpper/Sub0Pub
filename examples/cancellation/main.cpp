/** Sub0Pub Example: Publish Cancellation
 *
 * Demonstrates:
 *   - Calling cancel() from within receive() to stop further delivery
 *   - Subsequent publishes are not affected by previous cancellations
 *   - Cancellation is scoped to the current publish cycle
 */
#include "sub0pub/sub0pub.hpp"
#include <cstdio>

struct Command { int id; };

class CommandSource : public sub0::Publish<Command> {
public:
    void send(int id) { sub0::publish(this, Command{id}); }
};

// A handler that "claims" commands by cancelling delivery to later subscribers
class PrimaryHandler : public sub0::Subscribe<Command> {
public:
    void receive(const Command& cmd) noexcept override {
        if (cmd.id < 100) {
            std::printf("  [Primary] Handled command %d (cancelling further delivery)\n", cmd.id);
            cancel(); // Stop delivery to FallbackHandler
        } else {
            std::printf("  [Primary] Skipping command %d (letting fallback handle)\n", cmd.id);
        }
    }
};

// A fallback handler that only sees commands not claimed by PrimaryHandler
class FallbackHandler : public sub0::Subscribe<Command> {
public:
    void receive(const Command& cmd) noexcept override {
        std::printf("  [Fallback] Handling command %d\n", cmd.id);
    }
};

int main()
{
    std::printf("=== Publish Cancellation ===\n\n");

    CommandSource source;
    PrimaryHandler primary;
    FallbackHandler fallback;

    std::printf("Command 42 (< 100, primary claims it):\n");
    source.send(42);

    std::printf("\nCommand 200 (>= 100, falls through to fallback):\n");
    source.send(200);

    std::printf("\nCommand 7 (primary claims again):\n");
    source.send(7);

    return 0;
}
