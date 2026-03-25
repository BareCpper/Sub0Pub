# Sub0Pub Code Style Guide

Style conventions derived from the existing codebase. Follow these for consistency.

---

## Naming

| Element | Convention | Example |
|---------|-----------|---------|
| Namespaces | lowercase | `sub0`, `detail`, `utility` |
| Classes/Structs | PascalCase | `Broker`, `Subscribe`, `BinaryReader` |
| Template parameters | PascalCase with `_t` suffix for type params | `Data`, `Prefix_t`, `Header_t` |
| Member variables | camelCase with `_` suffix | `state_`, `publishCanceled_`, `ostream_` |
| Local variables | camelCase | `iSubscription`, `readCount` |
| Constants | `c` prefix + PascalCase | `cMaxSubscriptions`, `cMaxDataBufferCount` |
| Macros/Defines | UPPER_SNAKE_CASE with `SUB0PUB_` prefix | `SUB0PUB_TRACE`, `SUB0PUB_MAX_SUBSCRIPTIONS` |
| Free functions | camelCase | `publish()`, `cancel()` |
| Type aliases | PascalCase | `OStream`, `IStream`, `StreamSize` |

## Formatting

- **Indentation:** 4 spaces (no tabs)
- **Braces:** Opening brace on same line for control flow, next line for class/function definitions
- **Line width:** ~120 characters soft limit
- **Pointer/reference alignment:** `Type* name` (pointer with type), `const Type& name` (reference with type)

```cpp
// Class definition
class Broker
{
public:
    void publish(const Data& data) const noexcept
    {
        for (uint32_t i = 0U; i < count; ++i)
        {
            if (subscriptions[i]->filter(data))
                subscriptions[i]->receive(data);
        }
    }
};
```

## Templates

- Use angle brackets with space inside for readability: `template< typename Data >`
- CRTP pattern classes should document the Target type expectation
- Use `using` aliases over `typedef` for new code

## Documentation

- Doxygen-style comments with `/** */` for public API
- `@tparam`, `@param[in]`, `@return`, `@remark`, `@note`, `@warning` tags
- Inline `///<` for member variable documentation
- No documentation needed for obvious getters/setters

## Preprocessor

- Feature flags use `#ifndef` / `#define` / `#endif` pattern with default values
- Guard conditions: `#if SUB0PUB_FLAG` (not `#ifdef`)
- Include guard: `#ifndef CROG_SUB0PUB_HPP`

## Error Handling

- Use `assert()` guarded by `#if SUB0PUB_ASSERT` for debug checks
- Use `std::runtime_error` guarded by `#if __cpp_exceptions` for recoverable errors
- `noexcept` on all publish/receive hot-path functions

## Integer Types

- Use `<cstdint>` fixed-width types: `uint32_t`, `uint8_t`, `uint_fast16_t`
- Unsigned literals with `U` suffix: `0U`, `8U`
- Cast explicitly when narrowing: `static_cast<uint_fast16_t>(value)`

## Includes

- Standard library includes sorted alphabetically
- Project includes use quotes: `#include "sub0pub/sub0pub.hpp"`
- System includes use angle brackets: `#include <algorithm>`
