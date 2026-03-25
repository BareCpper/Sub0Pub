# Tagged Types Proposal

**Status:** Concept / Discussion | **Date:** 2026-03-25

## Problem

Sometimes a system needs to dispatch the same data type for different purposes. For example, log messages of varying severity, or sensor readings from different channels. Currently, Sub0Pub routes purely by C++ type — all `Subscribe<LogEntry>` receive all `LogEntry` publishes regardless of intent.

Users can work around this with `filter()`, but that means:
- Every subscriber receives every message and must inspect + reject at runtime
- No compile-time enforcement of which tags a subscriber cares about
- All subscribers pay the virtual call cost even for messages they'll never want

## Proposed Solution: `Tagged<Data, Tag>`

A wrapper type that creates a distinct pub/sub channel per tag:

```cpp
// Tag types (empty structs, zero-cost)
struct Error {};
struct Warning {};
struct Info {};

// Publisher sends tagged log entries
class Logger : public sub0::Publish<sub0::Tagged<LogEntry, Error>>
             , public sub0::Publish<sub0::Tagged<LogEntry, Warning>>
             , public sub0::Publish<sub0::Tagged<LogEntry, Info>>
{
public:
    void error(const char* msg) { sub0::publish(this, sub0::Tagged<LogEntry, Error>{{msg}}); }
    void warn(const char* msg)  { sub0::publish(this, sub0::Tagged<LogEntry, Warning>{{msg}}); }
    void info(const char* msg)  { sub0::publish(this, sub0::Tagged<LogEntry, Info>{{msg}}); }
};

// Subscriber receives only errors
class ErrorHandler : public sub0::Subscribe<sub0::Tagged<LogEntry, Error>> {
    void receive(const sub0::Tagged<LogEntry, Error>& tagged) noexcept override {
        handle(tagged.value); // .value is the unwrapped LogEntry
    }
};
```

### Implementation

```cpp
template<typename Data, typename Tag>
struct Tagged {
    Data value;
};
```

This is trivially implementable — `Tagged<LogEntry, Error>` and `Tagged<LogEntry, Warning>` are distinct C++ types, so the existing `Broker<T>` template instantiation creates separate channels automatically. Zero runtime cost, full compile-time routing.

## Approaches Considered

### Approach 1: Simple wrapper (above)

**Pros:**
- Zero implementation effort — just a struct template
- Fully compile-time, zero overhead
- Works with all existing Sub0Pub features (SubscribeAll, ForwardSubscribe, IPC)
- Users define their own tag types

**Cons:**
- Cannot subscribe to "all tags of LogEntry" without listing them explicitly
- Verbose: `sub0::Tagged<LogEntry, Error>` vs just `LogEntry`
- IPC serialization sees `Tagged<LogEntry, Error>` and `Tagged<LogEntry, Warning>` as completely different types with different typeHashes
- Each tag creates a separate Broker instance (memory for subscription tables)

### Approach 2: Runtime tag with compile-time filter

```cpp
template<typename Data, typename Tag = void>
struct Tagged {
    Data value;
    static constexpr uint32_t tag = sub0::utility::typeHash<Tag>();
};

// Subscribe to all LogEntry regardless of tag, with optional filter
class AllLogSubscriber : public sub0::Subscribe<Tagged<LogEntry>> { ... };

// Subscribe to only Error-tagged LogEntry
class ErrorSubscriber : public sub0::Subscribe<Tagged<LogEntry, Error>> { ... };
```

**Pros:**
- Can subscribe to "all tags" via `Subscribe<Tagged<LogEntry>>` (tag=void)
- Individual tags via `Subscribe<Tagged<LogEntry, Error>>`

**Cons:**
- Requires dual dispatch: `Broker<Tagged<LogEntry>>` AND `Broker<Tagged<LogEntry, Error>>`
- Publisher must publish to both the tagged and untagged brokers
- More complex, harder to reason about

### Approach 3: Tag as filter (current pattern, no new types)

```cpp
struct LogEntry { int level; const char* msg; };

class ErrorOnly : public sub0::Subscribe<LogEntry> {
    bool filter(const LogEntry& e) noexcept override { return e.level >= 3; }
    void receive(const LogEntry& e) noexcept override { /* ... */ }
};
```

**Pros:**
- Works today, no changes needed
- Flexible — runtime filtering on any field

**Cons:**
- Every subscriber pays the virtual `filter()` call cost
- No compile-time enforcement of tag categories
- Filter logic is spread across subscriber implementations

### Approach 4: Tag enum with compile-time dispatch

```cpp
enum class LogLevel { Debug, Info, Warning, Error };

template<typename Data, auto TagValue>
struct TaggedValue { Data value; };

// Distinct types per enum value
class ErrorSub : public sub0::Subscribe<TaggedValue<LogEntry, LogLevel::Error>> { ... };
```

**Pros:**
- Enum-based tags are discoverable and self-documenting
- Same zero-overhead as Approach 1

**Cons:**
- Requires C++17 `auto` non-type template parameter
- Same "cannot subscribe to all" limitation as Approach 1

## Recommendation

**Start with Approach 1** — it requires zero library changes and is just a documented pattern. Provide `sub0::Tagged<Data, Tag>` as a convenience alias in the header and a worked example.

For the "subscribe to all tags" use case (Approach 2), defer until there's a real product need. The complexity of dual-dispatch is significant and the use case may be adequately served by `filter()` in practice.

## Questions to Resolve

1. Should `Tagged<Data, Tag>` be part of the library or just a documented pattern?
2. Is IPC serialization of tagged types important? (typeHash would differ per tag)
3. Does `SubscribeAll<Tagged<LogEntry, Error>, Tagged<LogEntry, Warning>>` feel natural enough?
4. Would a `TaggedPublish<Data, Tags...>` helper that publishes to multiple tag channels be useful?
