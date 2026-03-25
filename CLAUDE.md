# Sub0Pub Project Rules

## Build & Test

```bash
cmake --preset default        # Configure
cmake --build --preset default  # Build
ctest --preset default          # Run tests
```

Benchmarks are built alongside tests but not run by ctest:
```bash
./build/tests/Release/Sub0Pub_Bench   # Windows
./build/tests/Sub0Pub_Bench           # Linux/macOS
```

## Commit Rules

### Migration Document
Any commit that changes the public API surface in `include/sub0pub/sub0pub.hpp` MUST update `MIGRATION.md` with the breaking change details. The public API includes:
- `sub0::Subscribe`, `sub0::Publish`, `sub0::SubscribeAll`
- `sub0::ForwardSubscribe`, `sub0::ForwardPublish`, `sub0::ForwardSubscribeAll`, `sub0::ForwardPublishAll`
- `sub0::StreamSerializer`, `sub0::StreamDeserializer`
- Free functions: `sub0::publish()`, `sub0::cancel()`
- Configuration macros: `SUB0PUB_*`
- `sub0::IPublish`, `sub0::Buffer`, `sub0::DefaultSerialisation`

### Style
Follow `STYLE_GUIDE.md` for all C++ code. Key points:
- 4 spaces, no tabs
- `noexcept` on all publish/receive hot-path functions
- `SUB0PUB_` prefix for all configuration macros

### Tests
- All new features must have corresponding tests in `tests/`
- Performance-sensitive changes should be validated with `Sub0Pub_Bench`
- Tests must pass locally before committing: `ctest --preset default`

## Branch Strategy
- `develop` — stable v1 baseline
- `v2` — active v2 development branch
- `v1.0` tag — final v1 state
