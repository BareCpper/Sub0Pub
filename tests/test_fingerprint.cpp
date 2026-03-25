#include "doctest.h"
#include "sub0pub/sub0pub.hpp"

namespace {

// Test structs with known member counts
struct Empty {};

struct OneField {
    int a;
};

struct TwoFields {
    int a;
    float b;
};

struct ThreeFields {
    int a;
    float b;
    double c;
};

struct Packed {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
};

struct WithPadding {
    uint8_t a;
    // padding
    uint32_t b;
};

// Simulates a "different version" of the same struct
struct TwoFieldsV2 {
    int a;
    float b;
    double c; // added field
};

} // namespace

TEST_CASE("Arity detection: member count") {
    CHECK(sub0::utility::memberCount<Empty> == 0);
    CHECK(sub0::utility::memberCount<OneField> == 1);
    CHECK(sub0::utility::memberCount<TwoFields> == 2);
    CHECK(sub0::utility::memberCount<ThreeFields> == 3);
    CHECK(sub0::utility::memberCount<Packed> == 4);
    CHECK(sub0::utility::memberCount<WithPadding> == 2);
}

TEST_CASE("Arity detection: primitive types") {
    // Primitives are brace-initializable with 1 value, so arity = 1
    CHECK(sub0::utility::memberCount<int> == 1);
    CHECK(sub0::utility::memberCount<float> == 1);
    CHECK(sub0::utility::memberCount<double> == 1);
}

TEST_CASE("TypeFingerprint: basic properties") {
    constexpr auto fp = sub0::utility::makeFingerprint<TwoFields>();
    CHECK(fp.size == sizeof(TwoFields));
    CHECK(fp.alignment == alignof(TwoFields));
    CHECK(fp.arity == 2);
}

TEST_CASE("TypeFingerprint: detects struct version mismatch") {
    constexpr auto v1 = sub0::utility::makeFingerprint<TwoFields>();
    constexpr auto v2 = sub0::utility::makeFingerprint<TwoFieldsV2>();

    // Different size, different arity — fingerprints must differ
    CHECK(v1 != v2);
    CHECK(v1.size != v2.size);
    CHECK(v1.arity != v2.arity);
}

TEST_CASE("TypeFingerprint: same struct matches itself") {
    constexpr auto a = sub0::utility::makeFingerprint<ThreeFields>();
    constexpr auto b = sub0::utility::makeFingerprint<ThreeFields>();
    CHECK(a == b);
}

TEST_CASE("TypeFingerprint: padding affects size but not arity") {
    constexpr auto packed = sub0::utility::makeFingerprint<Packed>();
    constexpr auto padded = sub0::utility::makeFingerprint<WithPadding>();

    // Both have different sizes due to padding
    CHECK(packed.size != padded.size);
    // Different arity
    CHECK(packed.arity != padded.arity);
}

// --- Member layout tests ---

struct SensorData {
    float temperature;
    uint32_t timestamp;
    uint8_t flags;
};

// Same members reordered — different offsets
struct SensorDataReordered {
    uint8_t flags;
    float temperature;
    uint32_t timestamp;
};

// Same names, added field
struct SensorDataV2 {
    float temperature;
    uint32_t timestamp;
    uint8_t flags;
    uint16_t quality;
};

TEST_CASE("SUB0_MEMBER_LAYOUT: basic layout capture") {
    constexpr auto layout = SUB0_MEMBER_LAYOUT(SensorData, temperature, timestamp, flags);

    CHECK(layout.fingerprint.size == sizeof(SensorData));
    CHECK(layout.fingerprint.alignment == alignof(SensorData));
    CHECK(layout.fingerprint.arity == 3);
    CHECK(layout.layoutHash != 0);
}

TEST_CASE("SUB0_MEMBER_LAYOUT: same struct matches itself") {
    constexpr auto a = SUB0_MEMBER_LAYOUT(SensorData, temperature, timestamp, flags);
    constexpr auto b = SUB0_MEMBER_LAYOUT(SensorData, temperature, timestamp, flags);
    CHECK(a == b);
}

TEST_CASE("SUB0_MEMBER_LAYOUT: detects member reordering") {
    constexpr auto original = SUB0_MEMBER_LAYOUT(SensorData, temperature, timestamp, flags);
    constexpr auto reordered = SUB0_MEMBER_LAYOUT(SensorDataReordered, flags, temperature, timestamp);

    // The basic fingerprint might match (same arity) but layout hash must differ
    // because offsets are different due to padding/reordering
    CHECK(original.layoutHash != reordered.layoutHash);
}

TEST_CASE("SUB0_MEMBER_LAYOUT: detects added field") {
    constexpr auto v1 = SUB0_MEMBER_LAYOUT(SensorData, temperature, timestamp, flags);
    constexpr auto v2 = SUB0_MEMBER_LAYOUT(SensorDataV2, temperature, timestamp, flags, quality);

    // Overall layout must differ (arity and/or layout hash)
    CHECK(v1 != v2);
    CHECK(v1.fingerprint.arity != v2.fingerprint.arity);
    // Note: sizeof may be equal due to padding reuse, but layoutHash catches the difference
    CHECK(v1.layoutHash != v2.layoutHash);
}

TEST_CASE("SUB0_MEMBER_LAYOUT: single member struct") {
    constexpr auto layout = SUB0_MEMBER_LAYOUT(OneField, a);

    CHECK(layout.fingerprint.arity == 1);
    CHECK(layout.fingerprint.size == sizeof(int));
    CHECK(layout.layoutHash != 0);
}

// 32-member struct — well beyond the old 16-member macro limit
struct Ridiculous {
    uint8_t  m00; uint8_t  m01; uint8_t  m02; uint8_t  m03;
    uint16_t m04; uint16_t m05; uint16_t m06; uint16_t m07;
    uint32_t m08; uint32_t m09; uint32_t m10; uint32_t m11;
    float    m12; float    m13; float    m14; float    m15;
    double   m16; double   m17; double   m18; double   m19;
    int8_t   m20; int8_t   m21; int8_t   m22; int8_t   m23;
    int16_t  m24; int16_t  m25; int16_t  m26; int16_t  m27;
    int32_t  m28; int32_t  m29; int64_t  m30; int64_t  m31;
};

TEST_CASE("SUB0_MEMBER_LAYOUT: 32 members (beyond old limit)") {
    constexpr auto layout = SUB0_MEMBER_LAYOUT(Ridiculous,
        m00, m01, m02, m03, m04, m05, m06, m07,
        m08, m09, m10, m11, m12, m13, m14, m15,
        m16, m17, m18, m19, m20, m21, m22, m23,
        m24, m25, m26, m27, m28, m29, m30, m31);

    CHECK(layout.fingerprint.size == sizeof(Ridiculous));
    CHECK(layout.fingerprint.alignment == alignof(Ridiculous));
    CHECK(layout.fingerprint.arity == 32);
    CHECK(layout.layoutHash != 0);

    // Same struct, same layout — must match
    constexpr auto layout2 = SUB0_MEMBER_LAYOUT(Ridiculous,
        m00, m01, m02, m03, m04, m05, m06, m07,
        m08, m09, m10, m11, m12, m13, m14, m15,
        m16, m17, m18, m19, m20, m21, m22, m23,
        m24, m25, m26, m27, m28, m29, m30, m31);

    CHECK(layout == layout2);
}
