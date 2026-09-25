#pragma once
/** Example project-wide configuration header (global default override).
 * Named by SUB0X_CONFIG_HEADER on the compiler command line (see CMakeLists.txt) so every TU sees it.
 * Zephyr would generate the equivalent from Kconfig (CONFIG_SUB0PUB_*).
 */
struct ProjectDefaults : sub0x::with<sub0x::Builtin, sub0x::Capacity<4>> {};
#define SUB0X_DEFAULT_CONFIG ProjectDefaults

#if defined(SUB0X_TEST_COUNT_MISMATCHES)
extern int gMismatches;
extern int gViolations;
#define SUB0X_CONFIG_MISMATCH(what) ((void)(what), ++gMismatches)
#define SUB0X_REENTRANT_VIOLATION(what) ((void)(what), ++gViolations)
#endif
