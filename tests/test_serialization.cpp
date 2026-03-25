#include "doctest.h"
#include "sub0pub/sub0pub.hpp"

#include <vector>
#include <cstring>

namespace {

// In-memory OStream for capturing serialized output
class MemoryOStream : public sub0::utility::OStream {
public:
    std::vector<char> data;

    StreamSize write(const char* const buffer, const StreamSize bufferCount) override {
        data.insert(data.end(), buffer, buffer + bufferCount);
        return bufferCount;
    }
    void flush() override {}
};

// In-memory IStream for replaying serialized data
class MemoryIStream : public sub0::utility::IStream {
public:
    const char* src;
    size_t remaining;

    MemoryIStream(const char* data, size_t size) : src(data), remaining(size) {}

    StreamSize read(char* const buffer, const StreamSize bufferCount) override {
        const size_t toRead = std::min(static_cast<size_t>(bufferCount), remaining);
        std::memcpy(buffer, src, toRead);
        src += toRead;
        remaining -= toRead;
        return static_cast<StreamSize>(toRead);
    }

    StreamSize readline(char* const buffer, const StreamSize bufferCount) override {
        return read(buffer, bufferCount);
    }

    StreamSize ignore(const StreamSize bufferCount) override {
        const size_t toSkip = std::min(static_cast<size_t>(bufferCount), remaining);
        src += toSkip;
        remaining -= toSkip;
        return static_cast<StreamSize>(toSkip);
    }

    StreamSize ignore(const StreamSize bufferCount, const char) override {
        return ignore(bufferCount);
    }

    bool isEof() override { return remaining == 0; }
};

// Serializer: subscribes to int and float, writes to stream
class TestSerializer : public sub0::StreamSerializer<>
                     , public sub0::ForwardSubscribe<int, TestSerializer>
                     , public sub0::ForwardSubscribe<float, TestSerializer>
{
public:
    TestSerializer(sub0::OStream& out) : sub0::StreamSerializer<>(out) {}
};

// Deserializer: reads from stream, publishes int and float
class TestDeserializer : public sub0::StreamDeserializer<>
                       , public sub0::ForwardPublish<int, TestDeserializer>
                       , public sub0::ForwardPublish<float, TestDeserializer>
{
public:
    TestDeserializer(sub0::IStream& in) : sub0::StreamDeserializer<>(in) {}
};

struct IntPublisher : sub0::Publish<int> {
    void send(int v) { sub0::publish(this, v); }
};

struct FloatPublisher : sub0::Publish<float> {
    void send(float v) { sub0::publish(this, v); }
};

struct IntReceiver : sub0::Subscribe<int> {
    int lastValue = 0;
    int count = 0;
    void receive(const int& v) noexcept override { lastValue = v; ++count; }
};

struct FloatReceiver : sub0::Subscribe<float> {
    float lastValue = 0.0f;
    int count = 0;
    void receive(const float& v) noexcept override { lastValue = v; ++count; }
};

} // namespace

TEST_CASE("Serialization round-trip: int") {
    MemoryOStream outStream;

    // Phase 1: Serialize (receiver not alive yet)
    {
        IntPublisher pub;
        TestSerializer serializer(outStream);
        pub.send(42);
        pub.send(-7);
    }

    CHECK(outStream.data.size() > 0);

    // Phase 2: Deserialize and re-publish (fresh receiver)
    IntReceiver receiver;
    MemoryIStream inStream(outStream.data.data(), outStream.data.size());
    {
        TestDeserializer deserializer(inStream);
        deserializer.open();
        while (deserializer.update()) {}
    }

    CHECK(receiver.count == 2);
    CHECK(receiver.lastValue == -7);
}

TEST_CASE("Serialization round-trip: multiple types") {
    MemoryOStream outStream;

    // Phase 1: Serialize (no receivers alive)
    {
        IntPublisher intPub;
        FloatPublisher floatPub;
        TestSerializer serializer(outStream);

        intPub.send(100);
        floatPub.send(3.14f);
        intPub.send(200);
    }

    CHECK(outStream.data.size() > 0);

    // Phase 2: Deserialize with fresh receivers
    IntReceiver intReceiver;
    FloatReceiver floatReceiver;
    MemoryIStream inStream(outStream.data.data(), outStream.data.size());
    {
        TestDeserializer deserializer(inStream);
        deserializer.open();
        while (deserializer.update()) {}
    }

    CHECK(intReceiver.count == 2);
    CHECK(intReceiver.lastValue == 200);
    CHECK(floatReceiver.count == 1);
    CHECK(floatReceiver.lastValue == doctest::Approx(3.14f));
}

TEST_CASE("Type hash uniqueness") {
    const auto intHash = sub0::utility::typeHash<int>();
    const auto floatHash = sub0::utility::typeHash<float>();
    const auto doubleHash = sub0::utility::typeHash<double>();

    CHECK(intHash != floatHash);
    CHECK(intHash != doubleHash);
    CHECK(floatHash != doubleHash);
}

TEST_CASE("DefaultSerialisation protocol structure") {
    // Verify that the serialized output has the expected framing
    MemoryOStream outStream;

    {
        TestSerializer serializer(outStream);
        IntPublisher pub;
        pub.send(42);
    }

    // Expected: Prefix(4 bytes 'SUB0') + Header(8 bytes) + Data(4 bytes int) + Postfix(1 byte '\n')
    const size_t expectedSize = sizeof(sub0::DefaultSerialisation::Prefix)
                              + sizeof(sub0::DefaultSerialisation::Header)
                              + sizeof(int)
                              + sizeof(sub0::DefaultSerialisation::Postfix);
    CHECK(outStream.data.size() == expectedSize);

    // Check magic prefix
    uint32_t magic = 0;
    std::memcpy(&magic, outStream.data.data(), sizeof(magic));
    CHECK(magic == sub0::utility::FourCC<'S', 'U', 'B', '0'>::value);

    // Check postfix delimiter
    CHECK(outStream.data.back() == '\n');
}
