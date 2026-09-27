/** Sub0Pub core header-only library
 * @remark C++ Type-based Subscriber-Publisher messaging model for embedded, desktop, games, and distributed systems.
 * 
 *  This file is part of Sub0Pub. Original project source available at https://github.com/Crog/Sub0Pub/blob/master/sub0pub.hpp
 * 
 *  MIT License
 *
 * Copyright (c) 2018 Craig Hutchinson <craig-sub0pub@crog.uk>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files 
 *  (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
 *  publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do 
 *  so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 *  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CROG_SUB0PUB_HPP
#define CROG_SUB0PUB_HPP

#include <algorithm>
#include <cstddef>
#include <atomic>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iosfwd>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#if SUB0PUB_THREAD_SAFE
#include <mutex>
#endif

/** Logging output for event tracing
 * Define SUB0PUB_TRACE=true to enable message logging to std::cout for event trace, SUB0PUB_TRACE=false
 */
#ifndef SUB0PUB_TRACE
#define SUB0PUB_TRACE false ///< Disable Trace logging to std::cout by default
#endif

/** Assertion based error handling 
 * Define SUB0PUB_ASSERT=true to enable assertion checks for events, SUB0PUB_ASSERT=false to disable
 */
#ifndef SUB0PUB_ASSERT
#define SUB0PUB_ASSERT true ///< Enable assertion tests by default
#endif

#ifndef SUB0PUB_STD
#define SUB0PUB_STD false ///< Use STD ostream and IStream types (May increase binary compiled size)
#endif

#ifndef SUB0PUB_TYPEIDNAME
#define SUB0PUB_TYPEIDNAME false ///< Types given unique/user-defined type index and string name for diagnostics and IPC
#endif

#ifndef SUB0PUB_THREAD_SAFE
#define SUB0PUB_THREAD_SAFE false ///< Optional mutex guard for multi-threaded pub/sub (e.g. FreeRTOS dual-core)
#endif

#ifndef SUB0PUB_MAX_SUBSCRIPTIONS
#define SUB0PUB_MAX_SUBSCRIPTIONS 8 ///< Fixed subscription table size per Broker<T>. Override globally or per-TU.
#endif

/* Default configuration: the cheapest correct dispatch. Every feature that costs something is opt-in, and
 * using one without opting in is detected: at compile time where possible, otherwise by a debug-build check.
 *   SUB0PUB_REENTRANT_SAFE  snapshot dispatch   detected by SUB0PUB_REENTRANT_CHECK (debug)
 *   SUB0PUB_CANCEL          publish context     cancel(), Route and publish reports do not compile without it
 *   SUB0PUB_FILTER          filter()            a subscriber declaring filter() does not compile without it
 *   SUB0PUB_THREAD_SAFE     lock                detected by SUB0PUB_THREAD_CHECK (debug)
 * These macros set the default for every Data type; one type can choose differently (sub0::config).
 */

#ifndef SUB0PUB_REENTRANT_SAFE
#define SUB0PUB_REENTRANT_SAFE false ///< Snapshot dispatch: subscribe or unsubscribe a Data type from inside its own
                                     ///< receive() (including destroying the receiving subscriber). Costs a copy of
                                     ///< the table and a publish context per publish.
#endif

#ifndef SUB0PUB_CANCEL
#define SUB0PUB_CANCEL false ///< Publish context: cancel(), Route (transport endpoints) and publish reports.
                             ///< Costs a thread_local frame per publish.
#endif

#ifndef SUB0PUB_FILTER
#define SUB0PUB_FILTER false ///< Subscribe<Data>::filter(): a virtual call per subscriber per publish.
#endif

/** Detect re-entrancy that Direct dispatch (SUB0PUB_REENTRANT_SAFE=false) does not support
 * Subscribing or unsubscribing (including destroying) a subscriber of a Data type from within a receive() of that
 * same Data type on the same thread is a contract violation of Direct dispatch; nested publish is supported. With
 * this check enabled the violation calls SUB0PUB_REENTRANT_VIOLATION(what).
 * Default: enabled in debug builds (SUB0PUB_ASSERT and no NDEBUG), disabled in release builds.
 * Define SUB0PUB_REENTRANT_CHECK true to keep the check in release builds (a thread_local frame per publish).
 * Has no effect with snapshot dispatch, which supports these.
 */
#ifndef SUB0PUB_REENTRANT_CHECK
#if SUB0PUB_ASSERT && !defined(NDEBUG)
#define SUB0PUB_REENTRANT_CHECK true
#else
#define SUB0PUB_REENTRANT_CHECK false
#endif
#endif

/** Action on a detected re-entrancy violation (see SUB0PUB_REENTRANT_CHECK)
 * @param what  Null-terminated description of the violation
 * Default asserts (debug) then aborts, so it also stops a release build that opted in to the check.
 * Override to log or count instead; if it returns, the operation continues unguarded.
 */
#ifndef SUB0PUB_REENTRANT_VIOLATION
#define SUB0PUB_REENTRANT_VIOLATION(what) do { assert(!(what)); std::abort(); } while(false)
#endif

/** Detect a Data type used from two threads at once without a lock
 * Publish, subscribe and unsubscribe of a Data type without a lock (SUB0PUB_THREAD_SAFE or sub0::LockWith) must not
 * overlap on different threads. With this check enabled an overlap calls SUB0PUB_THREAD_VIOLATION(what).
 * Default: enabled in debug builds (SUB0PUB_ASSERT and no NDEBUG). It detects overlaps that happen, not every race.
 */
#ifndef SUB0PUB_THREAD_CHECK
#if SUB0PUB_ASSERT && !defined(NDEBUG)
#define SUB0PUB_THREAD_CHECK true
#else
#define SUB0PUB_THREAD_CHECK false
#endif
#endif

/** Action on a detected unlocked concurrent use (see SUB0PUB_THREAD_CHECK). Default asserts, then aborts. */
#ifndef SUB0PUB_THREAD_VIOLATION
#define SUB0PUB_THREAD_VIOLATION(what) do { assert(!(what)); std::abort(); } while(false)
#endif

/** Debug diagnostic for a Data type resolving to different configurations in different translation units
 * Default: enabled in debug builds (SUB0PUB_ASSERT and no NDEBUG). Consistent configuration visibility is a build
 * contract (see sub0::config_t); this check only reports violations it observes at runtime.
 */
#ifndef SUB0PUB_CHECK_CONFIG
#if SUB0PUB_ASSERT && !defined(NDEBUG)
#define SUB0PUB_CHECK_CONFIG true
#else
#define SUB0PUB_CHECK_CONFIG false
#endif
#endif

/** Action on a detected configuration mismatch (see SUB0PUB_CHECK_CONFIG). Default asserts, then aborts. */
#ifndef SUB0PUB_CONFIG_MISMATCH
#define SUB0PUB_CONFIG_MISMATCH(what) do { assert(!(what)); std::abort(); } while(false)
#endif

/** Action when a Domain is destroyed while handles are still bound to it. Default asserts, then aborts. */
#ifndef SUB0PUB_DOMAIN_LIFETIME
#define SUB0PUB_DOMAIN_LIFETIME(what) do { assert(!(what)); std::abort(); } while(false)
#endif

/** Helper macro for stringifying value using compiler preprocessor
 * e.g. SUB0PUB_STRINGIFY_HELPER(123) == "123", SUB0PUB_STRINGIFY_HELPER(FooBar) == "FooBar"
 * @param  x  A value whos value will be converted to string e.g. FooBar == "FooBar", 123 = "123"
 */
#define SUB0PUB_STRINGIFY_HELPER(x) #x

/** Helper macro for stringifying define using compiler preprocessor
 * e.g. SUB0PUB_STRINGIFY_HELPER(__LINE__) == "123??"
 * @param  x  A macro definition whos value will be converted to string  e.g. __LINE__ == "123??"
 */
#define SUB0PUB_STRINGIFY(x) SUB0PUB_STRINGIFY_HELPER(x)

#if SUB0PUB_STD
#include <ostream> //< std::ostream
#include <istream> //< std::istream
#endif

/// @todo Trace interface - currently std::cout only!!
#if SUB0PUB_TRACE
#include <iostream>
#endif

/** Sub0Pub top-level namespace
 *
 * Header layout:
 *   1. Utility        - streams, hashing, arity detection, layout fingerprinting
 *   2. Configuration  - per-Data policy (capacity, dispatch, context, lock, filter, storage) and its resolution
 *   3. Core API       - Subscribe, Publish, SubscribeAll, Domain, Route, publish(), cancel()
 *   4. Static wiring  - wire(), StaticWiring, Sink, Publisher, Forward, DynamicPort, BrokerPort
 *   5. IPC API        - StreamSerializer, StreamDeserializer, ForwardSubscribe/Publish
*/
namespace sub0
{
    namespace detail
    {
        struct Empty {};
    }

// ============================================================================
// Section 1: Utility - Streams, hashing, arity detection, layout fingerprinting
// ============================================================================

    namespace utility
    {
        /** Create 4byte packed value at compile time
         * @tparam a,b,c,d  Characters which will be packed into 4-byte uint32_t value
         */
        template <const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d>
        struct FourCC
        {
            static constexpr uint32_t value = (((((d << 8) | c) << 8) | b) << 8) | a;
        };

        /** Hash a string using djb2 hash
         * @param[in] str  Null-terminated string to calculate hash of
         * @return djb2 hash value for input 'str'
         */
        constexpr uint32_t hash(const char* str)
        {
            uint32_t h = 5381U;
            for ( ; str[0U] != '\0'; ++str)
                h = ((h << 5) + h) + static_cast<uint32_t>(str[0U]);
            return h;
        }

        /** Compile-time unique type identifier using __PRETTY_FUNCTION__ / __FUNCSIG__
         * @tparam T  Type to generate a unique ID for
         * @return Unique uint32_t hash for type T, stable within a single build
         */
        template<typename T>
        constexpr uint32_t typeHash()
        {
#if defined(__GNUC__) || defined(__clang__)
            return hash(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
            return hash(__FUNCSIG__);
#else
            static_assert(false, "Sub0Pub: typeHash requires GCC, Clang, or MSVC");
#endif
        }

        /** Aggregate arity detection via structured bindings / aggregate init
         * @remark Detects the number of members in an aggregate type at compile time.
         *         Used to create a cheap struct-layout fingerprint for IPC verification.
         *         Only works for aggregate types (no user-declared constructors, no virtual functions).
         * @note   Technique from Boost.PFR / Antony Polukhin
         */
        namespace arity {
            // A type that is implicitly convertible to anything
            struct ubiq { template<typename T> operator T() const; };

            // Test whether T can be aggregate-initialized with N arguments
            template<typename T, typename Seq, typename = void>
            struct is_aggregate_constructible : std::false_type {};

            template<typename T, std::size_t... Is>
            struct is_aggregate_constructible<T, std::index_sequence<Is...>,
                std::void_t<decltype(T{ (void(Is), ubiq{})... })>>
                : std::true_type {};

            // Class-type variant: each ubiq is wrapped in its own braces so it initializes exactly one
            // direct member. A bare ubiq is brace-elided into array members (float[4] counting as 4),
            // over-counting the arity that structured bindings (layout::Decompose) see.
            template<typename T, typename Seq, typename = void>
            struct is_aggregate_constructible_braced : std::false_type {};

            template<typename T, std::size_t... Is>
            struct is_aggregate_constructible_braced<T, std::index_sequence<Is...>,
                std::void_t<decltype(T{ { (void(Is), ubiq{}) }... })>>
                : std::true_type {};

            template<typename T, std::size_t N>
            constexpr bool can_construct = std::is_class_v<T>
                ? is_aggregate_constructible_braced<T, std::make_index_sequence<N>>::value
                : is_aggregate_constructible<T, std::make_index_sequence<N>>::value;

            // Binary search for the maximum N where T{ubiq, ubiq, ..., ubiq} compiles
            template<typename T, std::size_t Lo, std::size_t Hi, typename = void>
            struct detect_impl {
                static constexpr std::size_t value = Lo;
            };

            template<typename T, std::size_t Lo, std::size_t Hi>
            struct detect_impl<T, Lo, Hi, std::enable_if_t<(Lo < Hi)>> {
                static constexpr std::size_t Mid = Lo + (Hi - Lo + 1) / 2;
                static constexpr std::size_t value =
                    can_construct<T, Mid>
                        ? detect_impl<T, Mid, Hi>::value
                        : detect_impl<T, Lo, Mid - 1>::value;
            };

            /// Upper bound capped at 32 to prevent MSVC template depth/heap exhaustion
            /// on large types (arrays, nested structs). 32 direct members covers
            /// virtually all IPC message types.
            static constexpr std::size_t MaxArity = 32;

            template<typename T>
            struct detect : detect_impl<T, 0, MaxArity> {};
        } // namespace arity

        /** Compile-time count of aggregate members in T
         * @tparam T  Aggregate type to count members of
         * @return Number of direct data members (0 for non-aggregate types)
         * @note Only valid for aggregate types (POD structs, C-style structs)
         */
        template<typename T>
        constexpr std::size_t memberCount = arity::detect<T>::value;

        /** Layout fingerprint combining sizeof, alignof, member count, and element info
         * @remark A cheap compile-time check for struct compatibility across IPC.
         *         Recursive: arrays include the element fingerprint, so changes to
         *         a struct used inside an array are always detected.
         */
        struct TypeFingerprint
        {
            uint32_t size;          ///< sizeof(T)
            uint32_t alignment;     ///< alignof(T)
            uint32_t arity;         ///< number of aggregate members (or 1 for scalars)
            uint32_t extent;        ///< array element count (0 for non-arrays)
            uint32_t elementHash;   ///< recursive fingerprint hash of element type (0 for non-arrays)

            bool operator==(const TypeFingerprint& rhs) const
            {
                return size == rhs.size && alignment == rhs.alignment
                    && arity == rhs.arity && extent == rhs.extent
                    && elementHash == rhs.elementHash;
            }

            bool operator!=(const TypeFingerprint& rhs) const
            { return !(*this == rhs); }
        };

        /** Hash a TypeFingerprint into a single uint32_t for embedding in parent fingerprints
         */
        constexpr uint32_t hashFingerprint(const TypeFingerprint& fp)
        {
            uint32_t h = 5381U;
            h = ((h << 5) + h) + fp.size;
            h = ((h << 5) + h) + fp.alignment;
            h = ((h << 5) + h) + fp.arity;
            h = ((h << 5) + h) + fp.extent;
            h = ((h << 5) + h) + fp.elementHash;
            return h;
        }

        /** Create a TypeFingerprint for any type at compile time
         * @remark Recursive: for array types T[N], the fingerprint includes
         *         the element type's fingerprint hash so that changes to nested
         *         structs are always detected through any depth of array nesting.
         */
        template<typename T>
        [[nodiscard]] constexpr TypeFingerprint makeFingerprint()
        {
            if constexpr (std::is_array_v<T>)
            {
                using Elem = std::remove_extent_t<T>;
                constexpr auto elemFp = makeFingerprint<Elem>();
                return { static_cast<uint32_t>(sizeof(T)),
                         static_cast<uint32_t>(alignof(T)),
                         static_cast<uint32_t>(elemFp.arity), //, Arity is from the Elem for arrays
                         static_cast<uint32_t>(std::extent_v<T>),
                         hashFingerprint(elemFp) };
            }
            else
            {
                return { static_cast<uint32_t>(sizeof(T)),
                         static_cast<uint32_t>(alignof(T)),
                         static_cast<uint32_t>(memberCount<T>),
                         0U, 0U };
            }
        }

        /** Per-member layout entry: offset, size, and recursive element hash
         */
        struct MemberEntry
        {
            uint32_t offset;      ///< byte offset from struct base
            uint32_t size;        ///< sizeof this member
            uint32_t elementHash; ///< recursive fingerprint hash (non-zero for arrays and structs with members)
        };

        /** Compute a hash over an array of MemberEntry for wire comparison
         * @remark Uses djb2 over offset, size, and elementHash of each member
         *         to produce a single uint32_t capturing the exact recursive
         *         byte layout of a struct.
         */
        constexpr uint32_t hashMemberLayout(const MemberEntry* entries, std::size_t count)
        {
            uint32_t h = 5381U;
            for (std::size_t i = 0; i < count; ++i)
            {
                h = ((h << 5) + h) + entries[i].offset;
                h = ((h << 5) + h) + entries[i].size;
                h = ((h << 5) + h) + entries[i].elementHash;
            }
            return h;
        }


        /** Extended layout fingerprint with per-member offset, size, and recursive element verification
         */
        struct TypeLayout
        {
            TypeFingerprint fingerprint; ///< sizeof + alignof + arity + array info
            uint32_t layoutHash;         ///< hash of per-member {offset, size, elementHash} triples

            bool operator==(const TypeLayout& rhs) const
            { return fingerprint == rhs.fingerprint && layoutHash == rhs.layoutHash; }

            bool operator!=(const TypeLayout& rhs) const
            { return !(*this == rhs); }
        };

        /** Create a MemberEntry from a structured binding reference
         * @remark Computes offset via pointer arithmetic from the struct base.
         *         Recursively fingerprints array and aggregate member types.
         */
        template<typename MemberT, typename BaseT>
        MemberEntry entryFrom(const BaseT& base, const MemberT& member)
        {
            using Raw = std::remove_cv_t<std::remove_reference_t<MemberT>>;
            const auto offset = static_cast<uint32_t>(
                reinterpret_cast<const char*>(&member) - reinterpret_cast<const char*>(&base));
            const auto size = static_cast<uint32_t>(sizeof(MemberT));
            uint32_t elemHash = 0;
            if constexpr (std::is_array_v<Raw> || (std::is_class_v<Raw> && memberCount<Raw> > 0))
                elemHash = hashFingerprint(makeFingerprint<Raw>());
            return { offset, size, elemHash };
        }

        /** Automatic layout decomposition via structured bindings (Boost.PFR-style)
         * @remark On GCC/Clang: uses class template partial specialization with
         *         structured bindings to decompose aggregates into per-member
         *         offset + size + recursive element hash.
         * @remark On MSVC: structured bindings in template specializations trigger
         *         eager parsing bugs (C3448). Falls back to TypeFingerprint only
         *         (sizeof+alignof+arity) without per-member offset detail.
         *         Full per-member support on MSVC awaits C++26 reflection.
         * @note   Supports up to 32 direct members (arity::MaxArity).
         */
        namespace layout {
            template<typename T, std::size_t N>
            struct Decompose { static uint32_t hash(T&) { return 0; } };

#if !defined(_MSC_VER)
            #define SUB0_E_(v, m) entryFrom(v, m)

            #define SUB0_LAYOUT_CASE(N, ...) \
                template<typename T> struct Decompose<T, N> { static uint32_t hash(T& v) { \
                    auto& [__VA_ARGS__] = v; \
                    MemberEntry e[] = {

            #define SUB0_LAYOUT_END(N) \
                    }; return hashMemberLayout(e, N); } };

            SUB0_LAYOUT_CASE(1,  m0) SUB0_E_(v,m0) SUB0_LAYOUT_END(1)
            SUB0_LAYOUT_CASE(2,  m0,m1) SUB0_E_(v,m0),SUB0_E_(v,m1) SUB0_LAYOUT_END(2)
            SUB0_LAYOUT_CASE(3,  m0,m1,m2) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2) SUB0_LAYOUT_END(3)
            SUB0_LAYOUT_CASE(4,  m0,m1,m2,m3) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3) SUB0_LAYOUT_END(4)
            SUB0_LAYOUT_CASE(5,  m0,m1,m2,m3,m4) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4) SUB0_LAYOUT_END(5)
            SUB0_LAYOUT_CASE(6,  m0,m1,m2,m3,m4,m5) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5) SUB0_LAYOUT_END(6)
            SUB0_LAYOUT_CASE(7,  m0,m1,m2,m3,m4,m5,m6) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6) SUB0_LAYOUT_END(7)
            SUB0_LAYOUT_CASE(8,  m0,m1,m2,m3,m4,m5,m6,m7) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7) SUB0_LAYOUT_END(8)
            SUB0_LAYOUT_CASE(9,  m0,m1,m2,m3,m4,m5,m6,m7,m8) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8) SUB0_LAYOUT_END(9)
            SUB0_LAYOUT_CASE(10, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9) SUB0_LAYOUT_END(10)
            SUB0_LAYOUT_CASE(11, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10) SUB0_LAYOUT_END(11)
            SUB0_LAYOUT_CASE(12, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11) SUB0_LAYOUT_END(12)
            SUB0_LAYOUT_CASE(13, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12) SUB0_LAYOUT_END(13)
            SUB0_LAYOUT_CASE(14, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13) SUB0_LAYOUT_END(14)
            SUB0_LAYOUT_CASE(15, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14) SUB0_LAYOUT_END(15)
            SUB0_LAYOUT_CASE(16, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15) SUB0_LAYOUT_END(16)
            SUB0_LAYOUT_CASE(17, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16) SUB0_LAYOUT_END(17)
            SUB0_LAYOUT_CASE(18, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17) SUB0_LAYOUT_END(18)
            SUB0_LAYOUT_CASE(19, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18) SUB0_LAYOUT_END(19)
            SUB0_LAYOUT_CASE(20, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19) SUB0_LAYOUT_END(20)
            SUB0_LAYOUT_CASE(21, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20) SUB0_LAYOUT_END(21)
            SUB0_LAYOUT_CASE(22, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21) SUB0_LAYOUT_END(22)
            SUB0_LAYOUT_CASE(23, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22) SUB0_LAYOUT_END(23)
            SUB0_LAYOUT_CASE(24, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23) SUB0_LAYOUT_END(24)
            SUB0_LAYOUT_CASE(25, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24) SUB0_LAYOUT_END(25)
            SUB0_LAYOUT_CASE(26, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25) SUB0_LAYOUT_END(26)
            SUB0_LAYOUT_CASE(27, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25,m26) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25),SUB0_E_(v,m26) SUB0_LAYOUT_END(27)
            SUB0_LAYOUT_CASE(28, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25,m26,m27) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25),SUB0_E_(v,m26),SUB0_E_(v,m27) SUB0_LAYOUT_END(28)
            SUB0_LAYOUT_CASE(29, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25,m26,m27,m28) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25),SUB0_E_(v,m26),SUB0_E_(v,m27),SUB0_E_(v,m28) SUB0_LAYOUT_END(29)
            SUB0_LAYOUT_CASE(30, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25,m26,m27,m28,m29) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25),SUB0_E_(v,m26),SUB0_E_(v,m27),SUB0_E_(v,m28),SUB0_E_(v,m29) SUB0_LAYOUT_END(30)
            SUB0_LAYOUT_CASE(31, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25,m26,m27,m28,m29,m30) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25),SUB0_E_(v,m26),SUB0_E_(v,m27),SUB0_E_(v,m28),SUB0_E_(v,m29),SUB0_E_(v,m30) SUB0_LAYOUT_END(31)
            SUB0_LAYOUT_CASE(32, m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16,m17,m18,m19,m20,m21,m22,m23,m24,m25,m26,m27,m28,m29,m30,m31) SUB0_E_(v,m0),SUB0_E_(v,m1),SUB0_E_(v,m2),SUB0_E_(v,m3),SUB0_E_(v,m4),SUB0_E_(v,m5),SUB0_E_(v,m6),SUB0_E_(v,m7),SUB0_E_(v,m8),SUB0_E_(v,m9),SUB0_E_(v,m10),SUB0_E_(v,m11),SUB0_E_(v,m12),SUB0_E_(v,m13),SUB0_E_(v,m14),SUB0_E_(v,m15),SUB0_E_(v,m16),SUB0_E_(v,m17),SUB0_E_(v,m18),SUB0_E_(v,m19),SUB0_E_(v,m20),SUB0_E_(v,m21),SUB0_E_(v,m22),SUB0_E_(v,m23),SUB0_E_(v,m24),SUB0_E_(v,m25),SUB0_E_(v,m26),SUB0_E_(v,m27),SUB0_E_(v,m28),SUB0_E_(v,m29),SUB0_E_(v,m30),SUB0_E_(v,m31) SUB0_LAYOUT_END(32)

            #undef SUB0_LAYOUT_CASE
            #undef SUB0_LAYOUT_END
            #undef SUB0_E_
#endif // !_MSC_VER
        } // namespace layout

        /** Create a TypeLayout automatically for any aggregate type
         * @remark Fully automatic — no macro or member list needed.
         *         Uses structured bindings (Boost.PFR-style) to decompose the
         *         struct and compute per-member offset + size + recursive element hash.
         * @note   Supports up to 32 direct members (arity::MaxArity).
         * @tparam T  Aggregate type to fingerprint
         */
        template<typename T>
        [[nodiscard]] TypeLayout makeLayout()
        {
            constexpr auto N = memberCount<T>;
            auto fp = makeFingerprint<T>();
            T val{};
            return { fp, layout::Decompose<T, N>::hash(val) };
        }

        /**
        * @note char* to unify interface against std::ostream
        */
        class OStream
        {
        public:
            typedef uint_fast32_t StreamSize;

            virtual StreamSize write(const char* const buffer, const StreamSize bufferCount) = 0;

            /** Clear all buffers for this stream and causes any buffered data to be written to the underlying device.
            */
            virtual void flush() = 0;
        };

        /**
        * @note char* to unify interface against std::istream
        */
        class IStream
        {
        public:
            typedef uint_fast32_t StreamSize;

            virtual StreamSize read(char* const buffer, const StreamSize bufferCount) = 0;

            /** Read stream line-by line until '\r', '\n', or '\r\n'
                @note Extends sub0::IStream
            */
            virtual StreamSize readline(char* const buffer, const StreamSize bufferCount) = 0;

            /** Discards specified number of characters from inputSequence
            * @note Setting std::numeric_limits<std::streamsize>::max() discards ONLY the currently buffered bytes
            * @return The number of bytes ignored
            */
            virtual StreamSize ignore( const StreamSize bufferCount ) = 0;

            /** Discards specified number of characters from inputSequence until the specified delimiter is found
            * @note The delimiting character is extracted, and thus the next input operation will continue on the character that follows it (if any).
            * @warning This function may (TBC) block if there isn't any data in the stream
            * @return The number of bytes ignored including the delimiter character
            */
            virtual StreamSize ignore(const StreamSize bufferCount, const char delimiter ) = 0;

            /** Returns whether end of stream has been reached
             * @note For Files this is explicit but for a pipe (e.g. TCP or command pipe '|' ) this may never occur until the pipe is forcefully closed by the other end etc
             * @return True if no more data, false otherwise
            */
            virtual bool isEof() = 0;
        };

// TODO: Need to refactor use of streams!?
#if SUB0PUB_STD
        /// @todo Determine how to avoid this i.e. Drop std::istream or only use interface type?
        inline size_t readline(std::istream& istream, char* const buffer, const size_t bufferCount)
        {
            return istream.getline(buffer, bufferCount).gcount();
        }

        template< typename Type_t >
        inline bool write(std::ostream& stream, const Type_t& value)
        {
            return stream.write(reinterpret_cast<const char*>(&value), sizeof(value)).good();
        }

        template< typename Type_t >
        inline bool write(std::ostream& stream)
        {
            const Type_t defaulted;
            return stream.write(reinterpret_cast<const char*>(&defaulted), sizeof(defaulted)).good();
        }

        template<>
        inline bool write<void>(std::ostream& stream)
        {
            return true;
        }
#else
        /// @todo Determine how to avoid this i.e. Drop std::istream or only use interface type?
        inline size_t readline(IStream& istream, char* const buffer, const size_t bufferCount)
        {
            return istream.readline(buffer, bufferCount);
        }

        template< typename Type_t >
        inline bool write(OStream& stream, const Type_t& value)
        {
            return stream.write(reinterpret_cast<const char*>(&value), sizeof(value)) == sizeof(value);
        }

        template< typename Type_t >
        inline bool write(OStream& stream)
        {
            const Type_t defaulted;
            return stream.write(reinterpret_cast<const char*>(&defaulted), sizeof(defaulted)) == sizeof(defaulted);
        }

        template<>
        inline bool write<void>(OStream& stream)
        {
            return true;
        }
#endif



        template< typename Type_t >
        constexpr size_t sizeOf() { return sizeof(Type_t); }

        template<>
        constexpr size_t sizeOf<void>() { return 0; }

        template< typename Type_t >
        constexpr void copyTo(char* buffer)
        { constexpr Type_t temp; std::memcpy(buffer, (const void*)&temp, sizeof(temp) ); }

        template< typename Type_t >
        constexpr void copyTo(char* buffer, const Type_t& value)
        { std::memcpy(buffer, (const void*)&value, sizeof(value)); }

        /// std::experimental::is_detected
        /// https://en.cppreference.com/w/cpp/experimental/is_detected
        namespace detail {
            template <class Default, class AlwaysVoid,
                template<class...> class Op, class... Args>
            struct detector {
                using value_t = std::false_type;
                using type = Default;
            };

            template <class Default, template<class...> class Op, class... Args>
            struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
                using value_t = std::true_type;
                using type = Op<Args...>;
            };

        } // namespace detail

        struct nonesuch {
            ~nonesuch() = delete;
            nonesuch(nonesuch const&) = delete;
            void operator=(nonesuch const&) = delete;
        };

        template <template<class...> class Op, class... Args>
        using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

        template <template<class...> class Op, class... Args>
        using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

        template <class Default, template<class...> class Op, class... Args>
        using detected_or_t = typename detail::detector<Default, void, Op, Args...>::type;

    } // END: utility


    // OStream/IStream type aliases (needed by IPC section below)
#if SUB0PUB_STD
    typedef std::ostream OStream;
    typedef std::istream IStream;
#else
    typedef utility::OStream OStream;
    typedef utility::IStream IStream;
#endif

// ============================================================================
// Section 2: Configuration — per-Data policy, resolved once per type
// ============================================================================

    /** Dispatch policy: how publish() walks the subscription table */
    enum class Dispatch : uint8_t
    {
        Snapshot,       ///< Copy the table before dispatch: re-entrant publish/subscribe/unsubscribe are safe
        Direct,         ///< Iterate the live table: fastest; re-entrant use of the same type's table is not supported
        DirectChecked   ///< Direct, and report re-entrant use through SUB0PUB_REENTRANT_VIOLATION (needs a context)
    };

    /** Publish context policy: per-dispatch state for cancel(), nesting, routes and same-thread disconnect */
    enum class Context : uint8_t
    {
        ThreadLocal,    ///< thread_local context: cancel() and nested publish on any thread
        Static,         ///< plain static context: as ThreadLocal for single-threaded images, without TLS
        None            ///< no context: no cancel(), no routes; the cheapest dispatch
    };

    /** Storage policy: where a type's subscription table lives */
    enum class Storage : uint8_t
    {
        Global,         ///< one table per Data type
        Scoped          ///< tables live in Domain<Data> instances passed at construction (independent sessions)
    };

    /** Lock policy for single-threaded use: an empty base, costs nothing */
    struct NoLock
    {
        void lock() noexcept {}
        void unlock() noexcept {}
    };

#if SUB0PUB_THREAD_SAFE
    /** Lock policy selected by SUB0PUB_THREAD_SAFE */
    struct StdMutexLock
    {
        void lock() noexcept { m.lock(); }
        void unlock() noexcept { m.unlock(); }
        std::mutex m;
    };
#endif

    namespace detail
    {
        template<class Data, class Config> class BrokerImpl; ///< the library broker (default implementation)

        /// Builtin configuration, named by the macro values it derives from: a translation unit that sets
        /// different SUB0PUB_* values for its own (TU-local) Data types gets a different type, not a second
        /// definition of the same one
        template<uint32_t Capacity, Dispatch D, Context C, bool Filter, class LockT>
        struct BuiltinT
        {
            /// Broker implementation (see Implementation<> and the broker concept on detail::BrokerImpl)
            template<class Data, class Config> using broker = BrokerImpl<Data, Config>;
            static constexpr uint32_t capacity = Capacity;
            static constexpr Dispatch dispatch = D;
            static constexpr Context context = C;
            static constexpr Storage storage = Storage::Global;
            static constexpr bool filter = Filter;
            using Lock = LockT;
        };
    }

    /** Builtin defaults: the configuration the SUB0PUB_* macros describe
     * Without any macro: Direct dispatch (DirectChecked in debug builds), no publish context, no filter(), no lock.
     */
    using Builtin = detail::BuiltinT<SUB0PUB_MAX_SUBSCRIPTIONS,
        (SUB0PUB_REENTRANT_SAFE || SUB0PUB_THREAD_SAFE) ? Dispatch::Snapshot
            : (SUB0PUB_REENTRANT_CHECK ? Dispatch::DirectChecked : Dispatch::Direct),
        (SUB0PUB_REENTRANT_SAFE || SUB0PUB_THREAD_SAFE || SUB0PUB_CANCEL) ? Context::ThreadLocal : Context::None,
        SUB0PUB_FILTER,
#if SUB0PUB_THREAD_SAFE
        StdMutexLock
#else
        NoLock
#endif
        >;

    // Options: each applies itself on top of a base configuration -----------------------------------------

    /// Fixed subscription table size
    template<uint32_t N> struct Capacity
    { template<class B> struct apply : B { static constexpr uint32_t capacity = N; }; };

    template<Dispatch D> struct DispatchWith
    { template<class B> struct apply : B { static constexpr Dispatch dispatch = D; }; };
    using Snapshot = DispatchWith<Dispatch::Snapshot>;
    using Direct = DispatchWith<Dispatch::Direct>;
    using DirectChecked = DispatchWith<Dispatch::DirectChecked>;

    template<Context C> struct ContextWith
    { template<class B> struct apply : B { static constexpr Context context = C; }; };
    using ThreadLocalContext = ContextWith<Context::ThreadLocal>;
    using StaticContext = ContextWith<Context::Static>;
    using NoContext = ContextWith<Context::None>;

    /// Any type with lock()/unlock() (and optionally a static yield()) makes the type safe for concurrent use.
    /// Concurrent use requires Snapshot dispatch and ThreadLocalContext, so this selects them too (a later option
    /// that contradicts them is a compile error).
    template<class L> struct LockWith
    {
        template<class B> struct apply : B
        {
            using Lock = L;
            static constexpr Dispatch dispatch = Dispatch::Snapshot;
            static constexpr Context context = Context::ThreadLocal;
        };
    };

    /// Subscribe<Data>::filter(): subscribers may skip a message (a virtual call per subscriber per publish)
    struct Filter
    { template<class B> struct apply : B { static constexpr bool filter = true; }; };

    /// No filter(): no per-subscriber filter call; a subscriber declaring filter() is then a compile error
    struct NoFilter
    { template<class B> struct apply : B { static constexpr bool filter = false; }; };

    /// Tables live in Domain<Data> instances passed to Subscribe/Publish at construction
    struct Scoped
    { template<class B> struct apply : B { static constexpr Storage storage = Storage::Scoped; }; };

    /// Replace the broker implementation for a Data type with an application-defined one (Global storage)
    template<template<class, class> class BrokerTemplate> struct Implementation
    { template<class B> struct apply : B { template<class Data, class Config> using broker = BrokerTemplate<Data, Config>; }; };

    namespace detail
    {
        template<class Base, class... Opts> struct fold { using type = Base; };
        template<class Base, class O, class... Rest>
        struct fold<Base, O, Rest...> : fold<typename O::template apply<Base>, Rest...> {};
    }

    /// Base configuration with options applied left to right (later options win)
    template<class Base, class... Opts>
    struct with : detail::fold<Base, Opts...>::type {};

} // END: sub0 (configuration vocabulary; the project header below may use it)

/** Project default configuration (resolution step 2)
 * Name a header with SUB0PUB_CONFIG_HEADER from the build system, so every translation unit agrees. It may
 * define e.g. `struct ProjectDefaults : sub0::with<sub0::Builtin, sub0::NoFilter> {};` and
 * `#define SUB0PUB_DEFAULT_CONFIG ProjectDefaults`.
 */
#if defined(SUB0PUB_CONFIG_HEADER)
#include SUB0PUB_CONFIG_HEADER
#endif

namespace sub0
{
#if defined(SUB0PUB_DEFAULT_CONFIG)
    using GlobalDefault = SUB0PUB_DEFAULT_CONFIG;
#else
    using GlobalDefault = Builtin;
#endif

    /** Per-Data configuration: the project default with options applied. The usual spelling at a Data site:
     *  `struct Imu { ...; using sub0_config = sub0::config<sub0::Capacity<2>, sub0::NoFilter>; };`
     */
    template<class... Opts>
    struct config : with<GlobalDefault, Opts...> {};

    /** Traits hook (resolution step 1c) for types that cannot carry a member alias or ADL declaration
     * @see SUB0PUB_CONFIGURE
     */
    template<class Data> struct configure {};

    namespace detail
    {
        /// Poison pill: ordinary lookup only ever finds this deleted template, so sub0_config is an ADL-only
        /// customisation point. A user's non-template sub0_config(T*) wins overload resolution against it.
        template<class T> void sub0_config(T*) = delete;

        /// Overload-based detection (portable to MSVC, which mishandles ADL inside void_t partial specialisations)
        template<class T> auto adl_probe(int) -> decltype(sub0_config(static_cast<T*>(nullptr)))*;
        template<class T> void adl_probe(...);

        template<class T, class = void> struct member_config { static constexpr bool found = false; };
        template<class T> struct member_config<T, std::void_t<typename T::sub0_config>>
        { static constexpr bool found = true; using type = typename T::sub0_config; };

        template<class T>
        struct adl_config
        {
            using probed = decltype(adl_probe<T>(0));
            static constexpr bool found = !std::is_void_v<probed>;
            using type = std::remove_pointer_t<probed>;
        };

        template<class T, class = void> struct traits_config { static constexpr bool found = false; };
        template<class T> struct traits_config<T, std::void_t<typename configure<T>::type>>
        { static constexpr bool found = true; using type = typename configure<T>::type; };

        template<class Data>
        struct resolve
        {
            static_assert(!std::is_reference_v<Data> && !std::is_const_v<Data>, "sub0pub: Data must be an unqualified object type");
            static constexpr int count = int(member_config<Data>::found) + int(adl_config<Data>::found) + int(traits_config<Data>::found);
            static_assert(count <= 1, "sub0pub: a Data type must be configured in exactly one place "
                                      "(member sub0_config, ADL sub0_config(Data*), or sub0::configure<Data>)");
            using type = std::conditional_t<member_config<Data>::found, member_config<Data>,
                         std::conditional_t<adl_config<Data>::found, adl_config<Data>,
                         std::conditional_t<traits_config<Data>::found, traits_config<Data>,
                         std::enable_if<true, GlobalDefault>>>>;
        };
    }

    /** The configuration every use of Data resolves to
     * Resolution: (1) exactly one of a member alias `Data::sub0_config`, an ADL declaration `sub0_config(Data*)`
     * or `sub0::configure<Data>` (SUB0PUB_CONFIGURE); else (2) SUB0PUB_DEFAULT_CONFIG; else (3) Builtin.
     * @warning Every translation unit must resolve the same configuration for a Data type: Subscribe<Data> depends on
     *          it, so resolving differently is an ODR violation. Member and ADL configuration are part of the type's
     *          definition and agree by construction; traits and the project header must be visible everywhere.
     */
    template<class Data>
    using config_t = typename detail::resolve<Data>::type::type;

    template<class Data> class Subscribe;
    template<class Data> class Publish;
    template<class Data> class Domain;

// ============================================================================
// Section 3: Core API — results, Subscribe, Publish, Domain, Route, publish(), cancel()
// ============================================================================

    /** Outcome of a bounded subscription registration
     * @see Subscribe::trySubscribe, Subscribe::isSubscribed
     */
    enum class SubscribeResult : uint8_t
    {
        Subscribed,        ///< Registered; the subscriber receives subsequent publishes
        CapacityExceeded,  ///< The table was full; table left unchanged
        Closed             ///< The subscriber's Domain has been closed
    };

    /** Outcome of handing a message to a transport. Acceptance is NOT remote delivery. */
    enum class SendResult : uint8_t
    {
        Accepted,          ///< The transport took the message (copied or serialized it)
        Full,              ///< Temporary: queue or buffer exhausted
        Disconnected,      ///< No peer at the moment
        Closed             ///< The transport is shutting down or shut down
    };

    /** Opt-in per-publish report of route results: sub0::publish(from, data, report)
     * @remark Local delivery is not affected by route results: every local subscriber is still called when a route rejects.
     */
    struct PublishReport
    {
        uint32_t routed = 0;
        uint32_t accepted = 0;
        uint32_t rejected = 0;
        SendResult lastRejection = SendResult::Accepted;

        void record(SendResult r) noexcept
        {
            ++routed;
            if (r == SendResult::Accepted)
                ++accepted;
            else
            {
                ++rejected;
                lastRejection = r;
            }
        }
    };

    namespace detail
    {
        /** Fingerprint of a configuration's effective values (not its type name) */
        template<class Config>
        constexpr uint32_t configFingerprint() noexcept
        {
            uint32_t h = 5381U;
            const uint32_t fields[] = {
                Config::capacity,
                static_cast<uint32_t>(Config::dispatch),
                static_cast<uint32_t>(Config::context),
                static_cast<uint32_t>(Config::storage),
                Config::filter ? 1U : 0U,
                utility::typeHash<typename Config::Lock>()
            };
            for (uint32_t f : fields)
                h = ((h << 5) + h) ^ f;
            return h | 1U; // never 0, which marks "unregistered"
        }

        /** Best-effort debug diagnostic for inconsistent configuration visibility across translation units
         * @warning Resolving a Data type differently in two TUs is an ODR violation and therefore undefined behaviour.
         *          Consistent visibility is a build contract; this registry only reports violations it observes.
         * @remark Registry<Data> does not depend on the configuration, so it is shared by all TUs.
         */
        template<class Data>
        struct Registry
        {
            inline static std::atomic<uint32_t> fingerprint{0};
        };

        template<class Data, class Config>
        void checkConfig() noexcept
        {
#if SUB0PUB_CHECK_CONFIG
            constexpr uint32_t mine = configFingerprint<Config>();
            uint32_t seen = 0;
            if (!Registry<Data>::fingerprint.compare_exchange_strong(seen, mine, std::memory_order_relaxed) && seen != mine)
                SUB0PUB_CONFIG_MISMATCH("sub0pub: Data type resolved to different configurations in different translation units");
#endif
        }

#if SUB0PUB_TYPEIDNAME
        /** User-assigned identity of a Data type for inter-process streams (SUB0PUB_TYPEIDNAME)
         * @remark Independent of the configuration, so shared by all translation units
         */
        template<class Data>
        struct TypeInfo
        {
            inline static uint32_t typeId = 0;
            inline static const char* typeName = nullptr;

            static void set(const uint32_t id, const char* const name) noexcept
            {
                if (id)
                {
#if SUB0PUB_ASSERT
                    assert(!typeId || typeId == id); // a Data type must be given one identifier
#endif
                    typeId = id;
                }
                if (name)
                {
#if SUB0PUB_ASSERT
                    assert(!typeName || std::strcmp(typeName, name) == 0); // and one name
#endif
                    typeName = name;
                }
            }
        };
#endif

        template<class Config>
        struct LockGuard
        {
            explicit LockGuard(typename Config::Lock& l) noexcept : l_(l) { l_.lock(); }
            ~LockGuard() { l_.unlock(); }
            LockGuard(const LockGuard&) = delete;
            LockGuard& operator=(const LockGuard&) = delete;
            typename Config::Lock& l_;
        };

        /// Whether a configuration is for concurrent use (has a Lock)
        template<class Config>
        constexpr bool cConcurrent = !std::is_same_v<typename Config::Lock, NoLock>;

        /** One dispatch in progress (concurrent configurations only), linked into its table under the table lock
         * @remark disconnect()/close() null a removed subscriber out of every active snapshot, then wait only while
         *         another thread is inside that subscriber's callback (`current`): bounded by one callback, no starvation.
         *         Dispatcher: store current, re-load entry; writer: store null entry, load current. Both seq_cst, so at
         *         least one side observes the other: a subscriber is never called after disconnect() returns.
         */
        template<class Data>
        struct ActiveDispatch
        {
            std::atomic<Subscribe<Data>*>* snapshot;
            uint32_t count;
            std::atomic<Subscribe<Data>*> current{nullptr};
            std::thread::id thread;
            ActiveDispatch* next = nullptr;
            ActiveDispatch* previous = nullptr;
        };

        template<class Data, bool Concurrent>
        struct ActiveList {};
        template<class Data>
        struct ActiveList<Data, true>
        {
            ActiveDispatch<Data>* activeHead = nullptr;

            void link(ActiveDispatch<Data>& d) noexcept
            {
                d.next = activeHead;
                if (activeHead)
                    activeHead->previous = &d;
                activeHead = &d;
            }
            void unlink(ActiveDispatch<Data>& d) noexcept
            {
                (d.previous ? d.previous->next : activeHead) = d.next;
                if (d.next)
                    d.next->previous = d.previous;
            }
        };

        /// Lifecycle state, only for Scoped storage: closed flag and count of bound handles
        template<bool Scoped>
        struct ScopeState {};
        template<>
        struct ScopeState<true>
        {
            bool closed = false;
            std::atomic<uint32_t> handles{0};
        };

        /// A subscriber's registration flag: atomic only where other threads may read it (a Lock)
        template<bool Atomic>
        struct Flag
        {
            bool load() const noexcept { return v_; }
            void store(bool b) noexcept { v_ = b; }
            bool exchange(bool b) noexcept { const bool old = v_; v_ = b; return old; }
            bool v_ = false;
        };
        template<>
        struct Flag<true>
        {
            bool load() const noexcept { return v_.load(std::memory_order_relaxed); }
            void store(bool b) noexcept { v_.store(b, std::memory_order_relaxed); }
            bool exchange(bool b) noexcept { return v_.exchange(b, std::memory_order_relaxed); }
            std::atomic<bool> v_{false};
        };

        /// Whether a configuration gets the debug check for unlocked concurrent use (SUB0PUB_THREAD_CHECK)
        template<class Config>
        constexpr bool cThreadCheck = SUB0PUB_THREAD_CHECK && !cConcurrent<Config>;

        /// A token unique to the calling thread (the address of a thread_local)
        inline std::uintptr_t threadToken() noexcept
        {
            thread_local const char anchor = 0;
            return reinterpret_cast<std::uintptr_t>(&anchor);
        }

        /// Table state for the unlocked-use check: which thread is using the table, 0 when none
        template<bool Enabled>
        struct ThreadCheck {};
        template<>
        struct ThreadCheck<true>
        {
            std::atomic<std::uintptr_t> user{0};
        };

        /** Marks one publish/subscribe/unsubscribe of an unlocked table; reports an overlap with another thread's
         *  (nested use on the same thread is fine). Empty unless SUB0PUB_THREAD_CHECK.
         */
        template<class TableT, bool Enabled>
        struct UseScope
        {
            explicit UseScope(TableT&) noexcept {}
        };
        template<class TableT>
        struct UseScope<TableT, true>
        {
            explicit UseScope(TableT& t) noexcept : t_(t)
            {
                const std::uintptr_t me = threadToken();
                std::uintptr_t seen = 0;
                acquired_ = t_.user.compare_exchange_strong(seen, me, std::memory_order_acquire, std::memory_order_relaxed);
                if (!acquired_ && seen != me)
                    SUB0PUB_THREAD_VIOLATION("sub0pub: a Data type was used from two threads at once without a lock: define "
                                             "SUB0PUB_THREAD_SAFE, or configure the type with sub0::LockWith<L>");
            }
            ~UseScope()
            {
                if (acquired_)
                    t_.user.store(0, std::memory_order_release);
            }
            UseScope(const UseScope&) = delete;
            UseScope& operator=(const UseScope&) = delete;
            TableT& t_;
            bool acquired_;
        };

        /// Subscription table for one Data type (Global) or one Domain (Scoped). Empty bases cost nothing.
        template<class Data, class Config>
        struct Table : Config::Lock, ActiveList<Data, cConcurrent<Config>>, ScopeState<Config::storage == Storage::Scoped>,
                      ThreadCheck<cThreadCheck<Config>>
        {
            uint32_t count = 0;
            Subscribe<Data>* entries[Config::capacity] = {};
        };

        /** One dispatch in progress on this thread. Frames form a per-thread stack (per Data type).
         * @remark `table` identifies the subscription table being dispatched, so cancel(), re-entrancy checks and
         *         disconnect act only on their own table (per Domain for Scoped storage). `origin` is the ingress
         *         binding that injected the message (split horizon); `report` collects route results (opt-in).
         */
        template<class Data>
        struct Frame
        {
            const void* table;
            const void* origin;
            PublishReport* report;
            Subscribe<Data>** snapshot; ///< this dispatch's snapshot (Snapshot dispatch), else nullptr
            uint32_t count;             ///< entries in snapshot
            bool canceled;
            Frame* previous;
        };

        /** Where a type's dispatch frames live: its configured context, or, for DirectChecked without one, a
         *  thread_local context that only the re-entrancy check uses (cancel() still needs a configured context)
         */
        template<class Config>
        constexpr Context frameContext = (Config::context == Context::None && Config::dispatch == Dispatch::DirectChecked)
                                         ? Context::ThreadLocal : Config::context;

        template<class Data, Context C>
        struct PublishContext
        {
            static constexpr bool enabled = false;
        };
        template<class Data>
        struct PublishContext<Data, Context::ThreadLocal>
        {
            static constexpr bool enabled = true;
            static Frame<Data>*& top() noexcept { return top_; }
            inline static thread_local Frame<Data>* top_ = nullptr;
        };
        template<class Data>
        struct PublishContext<Data, Context::Static>
        {
            static constexpr bool enabled = true;
            static Frame<Data>*& top() noexcept { return top_; }
            inline static Frame<Data>* top_ = nullptr;
        };

        template<class Lock, class = void> struct has_yield : std::false_type {};
        template<class Lock> struct has_yield<Lock, std::void_t<decltype(Lock::yield())>> : std::true_type {};

        /// Yield while quiescing: Lock::yield() if the lock type provides one (RTOS), else std::this_thread::yield()
        template<class Config>
        void yieldThread() noexcept
        {
            if constexpr (has_yield<typename Config::Lock>::value)
                Config::Lock::yield();
            else
                std::this_thread::yield();
        }

        /// Scope handle held by each Subscribe/Publish: empty for Global storage; Scoped counts bound handles
        template<class TableT, bool Scoped>
        struct ScopeRef
        {
            TableT* get(TableT& global) const noexcept { return &global; }
        };
        template<class TableT>
        struct ScopeRef<TableT, true>
        {
            explicit ScopeRef(TableT& t) noexcept : t_(&t) { t_->handles.fetch_add(1, std::memory_order_relaxed); }
            ~ScopeRef() { t_->handles.fetch_sub(1, std::memory_order_release); }
            ScopeRef(const ScopeRef&) = delete;
            ScopeRef& operator=(const ScopeRef&) = delete;
            TableT* get(TableT&) const noexcept { return t_; }
            TableT* t_;
        };
    }

    /** Broker author kit: what an application-defined broker (Implementation<>) builds on */
    namespace kit
    {
        /** RAII dispatch frame: push while calling receivers so cancel(), routes (origin, report) and same-thread
         * disconnect work. Zero-size when the Data type's configuration has no publish context.
         */
        template<class Data>
        class DispatchScope
        {
            using Ctx = detail::PublishContext<Data, detail::frameContext<config_t<Data>>>;
        public:
            DispatchScope(const void* table, const void* origin, PublishReport* report,
                          Subscribe<Data>** snapshot, uint32_t count) noexcept
                : frame_(makeFrame(table, origin, report, snapshot, count))
            {
                if constexpr (Ctx::enabled)
                    Ctx::top() = &frame_;
            }
            ~DispatchScope()
            {
                if constexpr (Ctx::enabled)
                    Ctx::top() = frame_.previous;
            }
            DispatchScope(const DispatchScope&) = delete;
            DispatchScope& operator=(const DispatchScope&) = delete;

            bool canceled() const noexcept
            {
                if constexpr (Ctx::enabled)
                    return frame_.canceled;
                else
                    return false;
            }

        private:
            struct NoFrame {};
            using FrameT = std::conditional_t<Ctx::enabled, detail::Frame<Data>, NoFrame>;

            /// Initialise the frame once, in place (no zero-fill then overwrite: avoids memset on small targets)
            static FrameT makeFrame(const void* table, const void* origin, PublishReport* report,
                                    Subscribe<Data>** snapshot, uint32_t count) noexcept
            {
                if constexpr (Ctx::enabled)
                    return FrameT{ table, origin, report, snapshot, count, false, Ctx::top() };
                else
                {
                    (void)table; (void)origin; (void)report; (void)snapshot; (void)count;
                    return FrameT{};
                }
            }

            FrameT frame_;
        };

        /// Call one subscriber: filter() (when configured) then receive(). Prefer deliverAt() in a dispatch loop.
        template<class Data>
        void deliver(Subscribe<Data>* s, const Data& data) noexcept;

        /** Call the subscriber held in a dispatch-owned slot (a snapshot entry, or a table entry for Direct dispatch):
         * filter() when configured, then receive() only if the slot still holds that subscriber. A filter() that
         * disconnects (or destroys) its own subscriber clears the slot, so receive() is not called. Only the slot is
         * re-read, never the subscriber. Without a filter this is exactly one load and one call.
         * @tparam MayBeCleared  false for a live Direct table entry, which is never null: skips that check
         */
        template<class Data, bool MayBeCleared = true, class Slot>
        void deliverAt(Slot& slot, const Data& data) noexcept;

        /// Innermost dispatch in progress on this thread for Data, or nullptr
        template<class Data>
        const detail::Frame<Data>* activeDispatch() noexcept
        {
            using Ctx = detail::PublishContext<Data, detail::frameContext<config_t<Data>>>;
            if constexpr (Ctx::enabled)
                return Ctx::top();
            else
                return nullptr;
        }

        /// Number of dispatches of `table` in progress on this thread
        template<class Data>
        uint32_t ownDispatches(const void* table) noexcept
        {
            uint32_t n = 0;
            for (const detail::Frame<Data>* f = activeDispatch<Data>(); f; f = f->previous)
                n += (f->table == table) ? 1U : 0U;
            return n;
        }

        /// Cancel the innermost dispatch of `table` on this thread; no-op if that table is not being dispatched
        template<class Data>
        void cancel(const void* table) noexcept
        {
            using Ctx = detail::PublishContext<Data, detail::frameContext<config_t<Data>>>;
            static_assert(config_t<Data>::context != Context::None,
                          "sub0pub: cancel() needs a publish context: define SUB0PUB_CANCEL, or configure the type with "
                          "sub0::ThreadLocalContext or sub0::StaticContext");
            if constexpr (Ctx::enabled)
                for (detail::Frame<Data>* f = Ctx::top(); f; f = f->previous)
                    if (f->table == table)
                    {
                        f->canceled = true;
                        return;
                    }
        }

        /// Remove `s` from this thread's in-progress snapshots of `table`, so a subscriber disconnected (and possibly
        /// destroyed) during a dispatch on this thread is not called afterwards by that dispatch
        template<class Data>
        void forgetInOwnDispatches(const void* table, const Subscribe<Data>* s) noexcept
        {
            using Ctx = detail::PublishContext<Data, detail::frameContext<config_t<Data>>>;
            if constexpr (Ctx::enabled)
                for (detail::Frame<Data>* f = Ctx::top(); f; f = f->previous)
                    if (f->table == table && f->snapshot)
                        for (uint32_t i = 0; i < f->count; ++i)
                            if (f->snapshot[i] == s)
                                f->snapshot[i] = nullptr;
        }
    }

    namespace detail
    {
        /// Subscriber interface, with filter() only when the configuration asks for it
        template<class Data, bool Filter>
        class SubscriberInterface
        {
        public:
            /** Receive published Data */
            virtual void receive(const Data& data) noexcept = 0;

            /** filter() is not enabled for this Data type: a subscriber that declares `bool filter(const Data&)` fails
             *  to compile here (conflicting return type / overrides a final function), with or without `override`,
             *  rather than being silently ignored. Enable it with SUB0PUB_FILTER or sub0::Filter. Never called.
             */
            virtual void filter(const Data&) noexcept final {}
        protected:
            ~SubscriberInterface() = default;
        };
        template<class Data>
        class SubscriberInterface<Data, true>
        {
        public:
            /** Receive published Data */
            virtual void receive(const Data& data) noexcept = 0;
            /** @return false to skip receive() for this message */
            virtual bool filter(const Data&) noexcept { return true; }
        protected:
            ~SubscriberInterface() = default;
        };

        /** The library broker for one Data type with one resolved configuration (the default Implementation)
         *
         * Broker concept (what Subscribe/Publish/Route require of any Implementation<>):
         *   Broker() noexcept                                            Global storage
         *   SubscribeResult trySubscribe(Subscribe<Data>*) noexcept
         *   void disconnect(Subscribe<Data>*) noexcept                   after return: no further receive() calls
         *   void publish(const Data&, const void* origin, PublishReport*) const noexcept
         *   void cancel() const noexcept
         * Deliver with kit::deliverAt() inside a kit::DispatchScope.
         */
        template<class Data, class Config>
        class BrokerImpl
        {
            static_assert(Config::capacity > 0, "sub0pub: Capacity must be at least 1");
            static_assert(!(Config::dispatch == Dispatch::Snapshot && Config::context == Context::None),
                          "sub0pub: Snapshot needs a publish context (StaticContext or ThreadLocalContext): a subscriber "
                          "disconnected during a dispatch is removed from that dispatch's snapshot through its frame");
            static_assert(!cConcurrent<Config> || Config::dispatch == Dispatch::Snapshot,
                          "sub0pub: a Lock requires Snapshot dispatch (receivers are called outside the lock)");
            static_assert(!cConcurrent<Config> || Config::context == Context::ThreadLocal,
                          "sub0pub: a Lock requires ThreadLocalContext (disconnect must not wait on its own dispatch, and a "
                          "StaticContext frame stack shared by concurrent publishers lets one thread's cancel() and "
                          "frames act on another thread's dispatch)");

            static constexpr bool cScoped = Config::storage == Storage::Scoped;

        public:
            using Configuration = Config;
            using TableT = Table<Data, Config>;
            static constexpr uint32_t cMaxSubscriptions = Config::capacity; ///< Subscription table size

            template<bool S = cScoped, std::enable_if_t<!S, int> = 0>
            BrokerImpl() noexcept { checkConfig<Data, Config>(); }

            template<bool S = cScoped, std::enable_if_t<S, int> = 0>
            explicit BrokerImpl(TableT& table) noexcept : scope_(table) { checkConfig<Data, Config>(); }

            SubscribeResult trySubscribe(Subscribe<Data>* subscriber) noexcept
            {
                TableT& t = table();
                LockGuard<Config> lk(t);
                UseScope<TableT, cThreadCheck<Config>> use(t);
                checkNotDispatching(t);
                if constexpr (cScoped)
                    if (t.closed)
                        return SubscribeResult::Closed;
                if (t.count >= Config::capacity)
                    return SubscribeResult::CapacityExceeded; // table unchanged: bounded, no out-of-bounds write
                t.entries[t.count++] = subscriber;
                return SubscribeResult::Subscribed;
            }

            /** Remove `subscriber`, keeping the order of the others; on return no dispatch (on any thread) calls it again
             * @remark Dispatches in progress forget it. Concurrent configurations then wait while another thread is
             *         inside its callback. Safe from within the subscriber's own receive().
             * @warning Concurrent: do not disconnect, from inside a receive(), a subscriber that another thread's
             *          receive() is disconnecting you from at the same time (mutual wait). Defer such teardown.
             */
            void disconnect(Subscribe<Data>* subscriber) noexcept
            {
                TableT& t = table();
                {
                    LockGuard<Config> lk(t);
                    UseScope<TableT, cThreadCheck<Config>> use(t);
                    // A plain search and shift: fewer instructions than std::find/std::move for a table this small
                    for (uint32_t i = 0; i < t.count; ++i)
                        if (t.entries[i] == subscriber)
                        {
                            checkNotDispatching(t);
                            for (uint32_t j = i + 1; j < t.count; ++j)
                                t.entries[j - 1] = t.entries[j];
                            --t.count;
                            break;
                        }
                    if constexpr (cConcurrent<Config>)
                        forgetInActiveDispatches(t, subscriber);
                }
                if constexpr (cConcurrent<Config>)
                    waitWhileCalledElsewhere(t, subscriber);
                else
                    kit::forgetInOwnDispatches<Data>(&t, subscriber);
            }

            void publish(const Data& data, const void* origin = nullptr, PublishReport* report = nullptr) const noexcept
            {
                TableT& t = table();
                if constexpr (cConcurrent<Config>)
                {
                    std::atomic<Subscribe<Data>*> snapshot[Config::capacity];
                    ActiveDispatch<Data> active{snapshot, 0, {nullptr}, std::this_thread::get_id()};
                    {
                        LockGuard<Config> lk(t);
                        if constexpr (cScoped)
                            if (t.closed)
                                return;
                        active.count = t.count;
                        for (uint32_t i = 0; i < active.count; ++i)
                            snapshot[i].store(t.entries[i], std::memory_order_relaxed);
                        t.link(active);
                    }
                    {
                        kit::DispatchScope<Data> scope(&t, origin, report, nullptr, 0);
                        for (uint32_t i = 0; !scope.canceled() && i < active.count; ++i)
                        {
                            Subscribe<Data>* const s = snapshot[i].load(std::memory_order_seq_cst);
                            if (s == nullptr)
                                continue;
                            active.current.store(s, std::memory_order_seq_cst);
                            kit::deliverAt<Data>(snapshot[i], data); // re-checks the slot: not disconnected meanwhile
                            active.current.store(nullptr, std::memory_order_seq_cst);
                        }
                    }
                    LockGuard<Config> lk(t);
                    t.unlink(active);
                }
                else if constexpr (Config::dispatch == Dispatch::Snapshot)
                {
                    UseScope<TableT, cThreadCheck<Config>> use(t);
                    Subscribe<Data>* snapshot[Config::capacity];
                    if constexpr (cScoped)
                        if (t.closed)
                            return;
                    const uint32_t count = t.count;
                    std::copy_n(t.entries, count, snapshot);
                    kit::DispatchScope<Data> scope(&t, origin, report, snapshot, count);
                    for (uint32_t i = 0; !scope.canceled() && i < count; ++i)
                        kit::deliverAt<Data>(snapshot[i], data);
                }
                else
                {
                    // Direct: a nested publish is fine; only changing this table during its own dispatch is not
                    UseScope<TableT, cThreadCheck<Config>> use(t);
                    kit::DispatchScope<Data> scope(&t, origin, report, nullptr, 0);
                    for (uint32_t i = 0; !scope.canceled() && i < t.count; ++i)
                        kit::deliverAt<Data, false>(t.entries[i], data);
                }
            }

            /// No-op without a publish context: Subscribe/Publish::cancel() reject that at compile time
            void cancel() const noexcept
            {
                if constexpr (Config::context != Context::None)
                    kit::cancel<Data>(&table());
            }

            /// Close a Scoped table: reject subscriptions, drop publishes, detach subscribers, then quiesce
            /// (a member template, so explicitly instantiating a Global broker does not instantiate it)
            template<bool S = cScoped, std::enable_if_t<S, int> = 0>
            static void close(TableT& t) noexcept;

        private:
            // The concurrent helpers are member templates, so explicitly instantiating a single-threaded broker
            // (e.g. to export it from a module) does not instantiate them

            /// Concurrent: null `s` (or every entry when s == nullptr) in all active snapshots. Call under the table lock.
            template<bool C = cConcurrent<Config>, std::enable_if_t<C, int> = 0>
            static void forgetInActiveDispatches(TableT& t, const Subscribe<Data>* s) noexcept
            {
                for (ActiveDispatch<Data>* a = t.activeHead; a; a = a->next)
                    for (uint32_t i = 0; i < a->count; ++i)
                        if (s == nullptr || a->snapshot[i].load(std::memory_order_relaxed) == s)
                            a->snapshot[i].store(nullptr, std::memory_order_seq_cst);
            }

            /// Concurrent: wait while another thread is inside `s`'s callback (any callback when s == nullptr)
            template<bool C = cConcurrent<Config>, std::enable_if_t<C, int> = 0>
            static void waitWhileCalledElsewhere(TableT& t, const Subscribe<Data>* s) noexcept
            {
                const std::thread::id me = std::this_thread::get_id();
                for (;;)
                {
                    bool busy = false;
                    {
                        LockGuard<Config> lk(t);
                        for (ActiveDispatch<Data>* a = t.activeHead; a && !busy; a = a->next)
                        {
                            Subscribe<Data>* const current = a->current.load(std::memory_order_seq_cst);
                            busy = a->thread != me && current != nullptr && (s == nullptr || current == s);
                        }
                    }
                    if (!busy)
                        return;
                    yieldThread<Config>();
                }
            }

            /// DirectChecked: only a use of the table currently being iterated on this thread is a violation
            static void checkNotDispatching(TableT& t) noexcept
            {
                if constexpr (Config::dispatch == Dispatch::DirectChecked)
                    if (kit::ownDispatches<Data>(&t) != 0)
                        SUB0PUB_REENTRANT_VIOLATION("sub0pub: subscribing or unsubscribing a Data type during its own dispatch requires Snapshot dispatch (SUB0PUB_REENTRANT_SAFE or sub0::Snapshot)");
                (void)t;
            }

            TableT& table() const noexcept { return *scope_.get(global_); }

            ScopeRef<TableT, cScoped> scope_;
            inline static TableT global_;
        };

        /// The broker implementation a Data type's configuration selects
        template<class Data>
        using BrokerFor = typename config_t<Data>::template broker<Data, config_t<Data>>;

        /// The broker every use of Data goes through
        template<class Data>
        using Broker = BrokerFor<Data>;
    }

    /** Session scope for Scoped types: independent subscription tables for the same Data type
     * @remark Lifetime contract: a Domain must outlive every Subscribe/Publish/Route bound to it (debug-checked).
     *         close() ends the session early: subscribe returns Closed, publish is dropped, current subscribers are
     *         detached, and in-flight dispatches are waited for.
     */
    template<class Data>
    class Domain
    {
        using Config = config_t<Data>;
        using BrokerT = detail::BrokerFor<Data>;
        static_assert(Config::storage == Storage::Scoped, "sub0pub: Domain<Data> requires a Data type configured with sub0::Scoped");
        static_assert(std::is_same_v<BrokerT, detail::BrokerImpl<Data, Config>>, "sub0pub: Scoped storage requires the library broker");
    public:
        Domain() = default;
        Domain(const Domain&) = delete;
        Domain& operator=(const Domain&) = delete;

        ~Domain()
        {
            close();
            if (table_.handles.load(std::memory_order_acquire) != 0)
                SUB0PUB_DOMAIN_LIFETIME("sub0pub: Domain destroyed while Subscribe/Publish/Route handles are still bound to it");
        }

        void close() noexcept { BrokerT::close(table_); }

        bool isClosed() const noexcept
        {
            detail::LockGuard<Config> lk(const_cast<typename BrokerT::TableT&>(table_));
            return table_.closed;
        }

    private:
        template<class> friend class Subscribe;
        template<class> friend class Publish;
        typename BrokerT::TableT table_;
    };

    /** Base type for an object that subscribes to some strong-typed Data
     * @tparam  Data  Type that will be received from publishers of corresponding type
     *
     * Interface: `void receive(const Data&) noexcept` (pure virtual), and `bool filter(const Data&) noexcept`
     * unless the type is configured with NoFilter.
     *
     * Activation contract: single-threaded configurations register in the constructor. Concurrent configurations
     * (a Lock, e.g. SUB0PUB_THREAD_SAFE) do not: another thread could otherwise dispatch into the object before the
     * derived class is constructed. Call trySubscribe() at the end of the most-derived constructor (Route does this).
     *
     * Teardown contract: after disconnect() returns, receive() is not called again, on any thread. The destructor
     * disconnects too, but by then the derived object is already destroyed: when other threads may publish, call
     * disconnect() from the most-derived destructor (Route does this). Same-thread disconnect during a dispatch,
     * including from the subscriber's own receive(), is safe with Snapshot dispatch.
     *
     * @remark The destructor is protected and non-virtual: a subscriber is destroyed as its own type, never through a
     *         Subscribe<Data>* (no vtable destructor slots, no operator delete dependency on small targets).
     */
    template<class Data>
    class Subscribe : public detail::SubscriberInterface<Data, config_t<Data>::filter>
    {
        using Config = config_t<Data>;
        using Broker = detail::BrokerFor<Data>;
        template<class> friend class Domain;
        template<class, class> friend class detail::BrokerImpl;
    public:
        /** Registers the subscriber (single-threaded configurations)
         * @param[in] typeId, typeName  Optional unique identity of Data for inter-process streams (SUB0PUB_TYPEIDNAME)
         */
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        Subscribe(
#if SUB0PUB_TYPEIDNAME
            const uint32_t typeId = 0, const char* typeName = nullptr
#endif
        ) noexcept
        {
#if SUB0PUB_TYPEIDNAME
            detail::TypeInfo<Data>::set(typeId, typeName);
#endif
            activateIfSingleThreaded();
        }

        /** Registers the subscriber in `domain` (Scoped types; single-threaded configurations) */
        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        explicit Subscribe(Domain<Data>& domain) noexcept : broker_(domain.table_) { activateIfSingleThreaded(); }

        Subscribe(const Subscribe&) = delete;
        Subscribe& operator=(const Subscribe&) = delete;

        /** @return Whether this subscriber is registered and will receive published Data
         * @remark False if the table was full (or the Domain closed) at registration, after disconnect(), or, for
         *         concurrent configurations, before trySubscribe().
         */
        bool isSubscribed() const noexcept { return subscribed_.load(); }

        /** Register, or retry registration after SubscribeResult::CapacityExceeded
         * @return SubscribeResult::Subscribed if now (or already) registered; CapacityExceeded with the table
         *         unchanged; Closed if the Domain has been closed
         */
        SubscribeResult trySubscribe() noexcept
        {
            if (isSubscribed())
                return SubscribeResult::Subscribed;
            const SubscribeResult result = broker_.trySubscribe(this);
            subscribed_.store(result == SubscribeResult::Subscribed);
            return result;
        }

        /// Stop receiving. Idempotent; safe from within receive(); see the teardown contract above
        void disconnect() noexcept
        {
            const bool wasSubscribed = subscribed_.exchange(false);
            // Concurrent: always, so a subscriber already detached by Domain::close() still waits out a callback in
            // progress on another thread. Single-threaded: close() already made it safe; nothing left to do.
            if (detail::cConcurrent<Config> || wasSubscribed)
                broker_.disconnect(this);
        }

        /** Stop delivery of the current publication to the remaining subscribers
         * @note Only meaningful from within receive() or filter()
         */
        void cancel() const noexcept
        {
            static_assert(Config::context != Context::None,
                          "sub0pub: cancel() needs a publish context: define SUB0PUB_CANCEL, or configure the type with "
                          "sub0::ThreadLocalContext or sub0::StaticContext");
            broker_.cancel();
        }

#if SUB0PUB_TYPEIDNAME
        /** @return Null-terminated name given to Data, or nullptr */
        const char* typeName() const noexcept { return detail::TypeInfo<Data>::typeName; }

        /** Stream operator for diagnostics reporting */
        friend OStream& operator<< (OStream& stream, const Subscribe<Data>& subscriber)
        { return stream << subscriber.typeName() << '{' << (const void*)&subscriber << '}'; }
#endif

    protected:
        ~Subscribe() { disconnect(); }

        void activateIfSingleThreaded() noexcept
        {
            if constexpr (!detail::cConcurrent<Config>)
                trySubscribe();
        }

        /// For bindings (Route): publish into this subscriber's table with an ingress origin
        void injectFrom(const void* origin, const Data& data) const noexcept { broker_.publish(data, origin, nullptr); }

    private:
        Broker broker_;
        detail::Flag<detail::cConcurrent<Config>> subscribed_;
    };

    namespace kit
    {
        template<class Data>
        void deliver(Subscribe<Data>* s, const Data& data) noexcept
        {
            if constexpr (config_t<Data>::filter)
                if (!s->filter(data))
                    return;
            s->receive(data);
        }

        namespace slot
        {
            template<class Data> Subscribe<Data>* load(Subscribe<Data>* const& p) noexcept { return p; }
            template<class Data> Subscribe<Data>* load(const std::atomic<Subscribe<Data>*>& p) noexcept { return p.load(std::memory_order_seq_cst); }
        }

        template<class Data, bool MayBeCleared, class Slot>
        void deliverAt(Slot& slot, const Data& data) noexcept
        {
            Subscribe<Data>* const s = slot::load<Data>(slot);
            if constexpr (MayBeCleared)
                if (s == nullptr)
                    return;
            if constexpr (config_t<Data>::filter)
            {
                if (!s->filter(data))
                    return;
                if (slot::load<Data>(slot) != s) // disconnected (or destroyed) inside its own filter()
                    return;
            }
            s->receive(data);
        }
    }

    template<class Data, class Config>
    template<bool S, std::enable_if_t<S, int>>
    void detail::BrokerImpl<Data, Config>::close(TableT& t) noexcept
    {
        uint32_t n;
        Subscribe<Data>* detached[Config::capacity];
        {
            LockGuard<Config> lk(t);
            UseScope<TableT, cThreadCheck<Config>> use(t);
            t.closed = true;
            n = t.count;
            std::copy_n(t.entries, n, detached);
            for (uint32_t i = 0; i < n; ++i)
                t.entries[i]->subscribed_.store(false);
            t.count = 0;
            if constexpr (cConcurrent<Config>)
                forgetInActiveDispatches(t, nullptr);
        }
        if constexpr (cConcurrent<Config>)
            waitWhileCalledElsewhere(t, nullptr);
        else
            for (uint32_t i = 0; i < n; ++i)
                kit::forgetInOwnDispatches<Data>(&t, detached[i]);
    }

    /**  Subscribe to many
    * @todo Specialisation on std::tuple exists and could cause unexpected expansion if this was a desired type being published!
    */
    template< typename... Datas >
    class SubscribeAll : public Subscribe<Datas>...
    {
    public:
        static constexpr size_t Count = sizeof...(Datas);
    };

    /**  Subscribe to many defined by std::tuple type list
    */
    template<typename... Datas>
    class SubscribeAll<std::tuple<Datas...>> : public Subscribe<Datas>...
    {
    public:
        static constexpr size_t Count = sizeof...(Datas);
    };

    /** Subscribe to many defined by multiple std::tuple type i.e. SubscribeAll< std::tuple<A,B>, std::tuple<B,C> >
    */
    template<typename... Datas, typename... OtherTuples>
    class SubscribeAll<std::tuple<Datas...>, OtherTuples...>
        : public SubscribeAll< decltype(std::tuple_cat( std::declval<std::tuple<Datas...>>(), std::declval<OtherTuples>()...)) >
    {};

    /** Base type for an object that publishes to some strong-typed Data
     * @tparam  Data  Type that will be published by this object to subscribers of corresponding type
     * @remark Not polymorphic: no virtual destructor and no vptr. For Global storage it is an empty handle.
     */
    template<class Data>
    class Publish
    {
        using Config = config_t<Data>;
        using Broker = detail::BrokerFor<Data>;
    public:
        /** @param[in] typeId, typeName  Optional unique identity of Data for inter-process streams (SUB0PUB_TYPEIDNAME) */
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        Publish(
#if SUB0PUB_TYPEIDNAME
            const uint32_t typeId = 0, const char* typeName = nullptr
#endif
        ) noexcept
        {
#if SUB0PUB_TYPEIDNAME
            detail::TypeInfo<Data>::set(typeId, typeName);
#endif
        }

        /** Publish into `domain` (Scoped types) */
        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        explicit Publish(Domain<Data>& domain) noexcept : broker_(domain.table_) {}

        /** Cancel the active publication of Data, stopping delivery to the remaining subscribers
         * @note Only meaningful from within a receive() callback
         */
        void cancel() const noexcept
        {
            static_assert(Config::context != Context::None,
                          "sub0pub: cancel() needs a publish context: define SUB0PUB_CANCEL, or configure the type with "
                          "sub0::ThreadLocalContext or sub0::StaticContext");
            broker_.cancel();
        }

#if SUB0PUB_TYPEIDNAME
        /** @return Null-terminated name given to Data, or nullptr */
        const char* typeName() const noexcept { return detail::TypeInfo<Data>::typeName; }

        /** @return Unique identifier given to Data, or 0 */
        uint32_t typeId() const noexcept { return detail::TypeInfo<Data>::typeId; }

        /** Stream operator for diagnostics reporting */
        friend OStream& operator<< (OStream& stream, const Publish<Data>& publisher)
        { return stream << publisher.typeName() << '{' << (const void*)&publisher << '}'; }
#endif

    protected:
        /** Publish data to subscribers
         * @note Protected: use the free function sub0::publish(*this, data) from derived classes
         */
        void publish(const Data& data, PublishReport* report = nullptr) const noexcept { broker_.publish(data, nullptr, report); }

    private:
        template<class From, class D> friend void publish(From&, const D&) noexcept;
        template<class From, class D> friend void publish(From&, const D&, PublishReport&) noexcept;
        Broker broker_;
    };

    /** Publish data, used when inheriting from multiple Publish<> base types
     * @remark Circumvents C++ name hiding when multiple Publish<> bases are present (publish(1.0F) would be ambiguous)
     * @note Compile error if From does not inherit Publish<Data>
     * @param[in] from  Producer object inheriting from one or more Publish<> objects
     * @param[in] data  Data that will be published using the base Publish<Data> object of From
     */
    template<class From, class Data>
    inline void publish(From& from, const Data& data) noexcept
    {
        const Publish<Data>& publisher = from;
        publisher.publish(data);
    }

    /** Publish and report route results (routed / accepted / rejected). Local delivery is unaffected by rejections. */
    template<class From, class Data>
    inline void publish(From& from, const Data& data, PublishReport& report) noexcept
    {
        static_assert(config_t<Data>::context != Context::None, "sub0pub: publish reports need a publish context");
        const Publish<Data>& publisher = from;
        publisher.publish(data, &report);
    }

    /** @see publish(From&, const Data&) */
    template<class From, class Data>
    inline void publish(From* const from, const Data& data) noexcept
    {
#if SUB0PUB_ASSERT
        assert(from != nullptr);
#endif
        publish(*from, data);
    }

    /** Cancel the active publication of Data on a publisher
     * @param[in] from  Producer object inheriting from Publish<Data>
     * @note Only meaningful from within a receive() callback
     */
    template<class Data, class From>
    inline void cancel(From& from) noexcept
    {
        const Publish<Data>& publisher = from;
        publisher.cancel();
    }

    /** Endpoint binding: connects one application-owned Transport instance to one table (a Domain, or the global
     * table) for one Data type.
     *
     *   Egress:  every message published into the table is handed to transport.send(data) -> SendResult.
     *            The transport must copy/serialize at acceptance and never retain a reference to `data`.
     *   Ingress: inject(data) publishes a message received from the transport into the table. That message is
     *            not sent back out through this route (split horizon), preventing echo loops between peers.
     *   Teardown: the destructor disconnects first, so after destruction the transport is never called.
     *
     * Transport concept: SendResult send(const Data&) noexcept.
     */
    template<class Data, class Transport>
    class Route final : public Subscribe<Data>
    {
        using Config = config_t<Data>;
        static_assert(Config::context != Context::None, "sub0pub: Route needs a publish context (split horizon and reports)");
    public:
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        explicit Route(Transport& transport) noexcept : transport_(transport) { this->trySubscribe(); }

        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        Route(Domain<Data>& domain, Transport& transport) noexcept : Subscribe<Data>(domain), transport_(transport) { this->trySubscribe(); }

        ~Route() { this->disconnect(); }

        /// Ingress: deliver a message received from the transport to this route's table
        void inject(const Data& data) const noexcept { this->injectFrom(this, data); }

    private:
        void receive(const Data& data) noexcept override
        {
            const detail::Frame<Data>* const frame = kit::activeDispatch<Data>();
            if (frame != nullptr && frame->origin == this)
                return; // split horizon: this message arrived through this route
            const SendResult result = transport_.send(data);
            if (frame != nullptr && frame->report != nullptr)
                frame->report->record(result);
        }

        Transport& transport_;
    };

    /** A payload type made distinct by a tag, which also carries its configuration:
     *  `struct Rpm { using sub0_config = sub0::config<sub0::Capacity<2>>; }; using RpmMsg = sub0::Tagged<int, Rpm>;`
     */
    template<class Data, class Tag>
    struct Tagged
    {
        Data value;
    };
    namespace detail
    {
        template<class Tag, class = void> struct tag_config {};
        template<class Tag> struct tag_config<Tag, std::void_t<typename Tag::sub0_config>> { using type = typename Tag::sub0_config; };
    }
    template<class Data, class Tag>
    struct configure<Tagged<Data, Tag>> : detail::tag_config<Tag> {};


// ============================================================================
// Section 4: Static wiring — typed bindings at the composition point
// ============================================================================
//
// Receivers are ordinary classes: a non-virtual `receive(const T&)` per message type they handle, and optionally
// `bool filter(const T&)`. No base class, no registry, no registration. The application binds concrete receiver
// instances where it composes itself; their types are kept all the way to the call, so each delivery is a direct,
// inlinable call (measured equal to hand-written code: docs/design/COLLAPSE_SCORES.md):
//
//   auto bus = sub0::wire(controllerA, controllerB, logger);             // runtime addresses, static types
//   using Bus = sub0::StaticWiring<&controllerA, &controllerB, &logger>; // static storage: no RAM, fixed targets
//   struct Sensor { sub0::Sink<Sample> out; ... };                       // non-template publisher: one indirect call
//
// Routing is by capability: publish(msg) calls, in bound order, every bound receiver that has receive(const T&).
// A receiver whose receive() returns bool stops the rest of a publishCancelable() by returning false.
// Messages never list receivers: per-message policy (Section 2) and application wiring stay separate.
// Wirings add no synchronisation: concurrent publishers need stable bindings and thread-safe receivers.

    namespace detail
    {
    namespace wiring
    {
        template<class R, class T, class = void> struct accepts : std::false_type {};
        template<class R, class T>
        struct accepts<R, T, std::void_t<decltype(std::declval<R&>().receive(std::declval<const T&>()))>> : std::true_type {};

        template<class R, class T, class = void> struct has_filter : std::false_type {};
        template<class R, class T>
        struct has_filter<R, T, std::void_t<decltype(bool(std::declval<R&>().filter(std::declval<const T&>())))>> : std::true_type {};

        /// Bound objects may be the receiver itself or a holder exposing it via get() (e.g. application storage slots)
        template<class X, class = void> struct has_get : std::false_type {};
        template<class X> struct has_get<X, std::void_t<decltype(std::declval<X&>().get())>> : std::true_type {};

        template<class X>
        constexpr decltype(auto) receiver(X& x) noexcept
        {
            if constexpr (has_get<X>::value)
                return x.get();
            else
                return (x);
        }

        template<class R, class T>
        using receive_result_t = decltype(std::declval<R&>().receive(std::declval<const T&>()));

        template<class R, class T, class = void> struct receive_returns_bool : std::false_type {};
        template<class R, class T>
        struct receive_returns_bool<R, T, std::enable_if_t<std::is_same_v<receive_result_t<R, T>, bool>>> : std::true_type {};

        /// Deliver to one receiver: nothing at all if it does not handle T; its filter only if it declares one
        template<class R, class T>
        inline void deliver(R& r, const T& msg) noexcept
        {
            if constexpr (accepts<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return;
                r.receive(msg);
            }
        }

        /// Deliver to one receiver; returns whether the publication continues. A receiver that does not handle T,
        /// is filtered out, or returns void always continues; one returning bool stops it with false.
        template<class R, class T>
        inline bool deliverContinue(R& r, const T& msg) noexcept
        {
            if constexpr (accepts<R, T>::value)
            {
                if constexpr (has_filter<R, T>::value)
                    if (!r.filter(msg))
                        return true;
                if constexpr (receive_returns_bool<R, T>::value)
                    return r.receive(msg);
                else
                {
                    r.receive(msg);
                    return true;
                }
            }
            else
                return true;
        }

        /// A binding adapter that only refers to the real endpoint (e.g. Forward<Transport>) declares
        /// `using sub0_by_value = void;` so a Wiring holds it by value: one hop to the endpoint, as hand-written
        template<class B, class = void> struct by_value : std::false_type {};
        template<class B> struct by_value<B, std::void_t<typename B::sub0_by_value>> : std::true_type {};
        template<class B> using stored_t = std::conditional_t<by_value<B>::value, B, B&>;

        /// Endpoint identity for split horizon: the bound object, or for an adapter what it refers to
        template<class X, class = void> struct has_identity : std::false_type {};
        template<class X> struct has_identity<X, std::void_t<decltype(std::declval<const X&>().sub0_identity())>> : std::true_type {};
        template<class X>
        constexpr const void* identity(const X& x) noexcept
        {
            if constexpr (has_identity<X>::value)
                return x.sub0_identity();
            else
                return static_cast<const void*>(&x);
        }

        template<class X> using receiver_t = std::remove_cv_t<std::remove_reference_t<decltype(receiver(std::declval<X&>()))>>;

        /// The endpoint a binding stands for: an adapter that forwards to a transport declares
        /// `using sub0_endpoint = Transport;`, so ingress may name either the adapter or the transport as its origin
        template<class X, class = void> struct endpoint_of { using type = X; };
        template<class X> struct endpoint_of<X, std::void_t<typename X::sub0_endpoint>> { using type = typename X::sub0_endpoint; };

        /// Whether a binding of type R is the endpoint an ingress origin of type Origin refers to
        template<class R, class Origin>
        constexpr bool isOrigin = std::is_same_v<std::remove_cv_t<R>, std::remove_cv_t<Origin>> ||
                                  std::is_same_v<typename endpoint_of<std::remove_cv_t<R>>::type, std::remove_cv_t<Origin>>;

        template<class Origin, class... R>
        constexpr std::size_t countOf = (std::size_t(isOrigin<R, Origin>) + ... + std::size_t(0));

        /// Split horizon: do not send a message back to the binding it came from. When the origin's type is bound
        /// exactly once (OriginUnique), the origin *is* that binding: decided at compile time, no address compare
        /// (precondition: the origin is one of the bound endpoints, asserted in debug builds).
        template<bool OriginUnique, class R, class T, class Origin>
        inline void deliverExcept(R& r, const T& msg, const Origin& origin) noexcept
        {
            if constexpr (isOrigin<R, Origin>)
            {
                if constexpr (OriginUnique)
                {
                    assert(identity(r) == identity(origin) && "publishFrom: origin is not a bound endpoint");
                    (void)origin;
                    return;
                }
                else if (identity(r) == identity(origin))
                    return;
            }
            deliver(r, msg);
        }

        /// Origin identified by type alone (publishFrom<Origin>(msg)): the one binding of that type is skipped
        template<class Origin, class R, class T>
        inline void deliverExceptType(R& r, const T& msg) noexcept
        {
            if constexpr (!isOrigin<R, Origin>)
                deliver(r, msg);
        }
    } // END: wiring
    } // END: detail

    /** Whether a wiring delivers T to R: a receiver meant to handle T can state it where it is bound, e.g.
     *      static_assert(sub0::handles_v<Logger, Sample>, "Logger must receive Sample");
     *  Capability routing is otherwise silent about a signature mismatch (known issue K14). Pass `const R` for a
     *  receiver bound through a pointer to const.
     */
    template<class R, class T>
    constexpr bool handles_v =
        detail::wiring::accepts<std::remove_reference_t<decltype(detail::wiring::receiver(std::declval<R&>()))>, T>::value;

    /** Receivers bound by reference at the composition point (runtime addresses, static types)
     * @see wire()
     */
    template<class... Bound>
    class Wiring
    {
    public:
        constexpr explicit Wiring(Bound&... bound) noexcept : bound_(bound...) {}

        /// Deliver to every bound receiver that handles T, in bound order
        template<class T>
        void publish(const T& msg) const noexcept { publish(msg, Indices{}); }

        /// As publish(), stopping at the first receiver whose bool receive() returns false
        template<class T>
        void publishCancelable(const T& msg) const noexcept { publishCancelable(msg, Indices{}); }

        /// Ingress from one of the bound receivers (e.g. a transport endpoint): every other receiver gets it
        template<class T, class Origin>
        void publishFrom(const Origin& origin, const T& msg) const noexcept { publishFrom(origin, msg, Indices{}); }

        /// Ingress identified by the origin's type, which must be bound exactly once: no origin object needed
        template<class Origin, class T>
        void publishFrom(const T& msg) const noexcept
        {
            static_assert(detail::wiring::countOf<Origin, detail::wiring::receiver_t<Bound>...> == 1,
                          "publishFrom<Origin>: Origin must be bound exactly once; pass the origin object instead");
            publishFromType<Origin>(msg, Indices{});
        }

    private:
        // Each binding is read (std::get) right before its own delivery, as hand-written code does. std::apply
        // would read every binding up front and keep them live across the calls (extra saved registers).
        using Indices = std::index_sequence_for<Bound...>;

        template<class T, std::size_t... I>
        void publish(const T& msg, std::index_sequence<I...>) const noexcept
        {
            (detail::wiring::deliver(detail::wiring::receiver(std::get<I>(bound_)), msg), ...);
        }

        template<class T, std::size_t... I>
        void publishCancelable(const T& msg, std::index_sequence<I...>) const noexcept
        {
            (detail::wiring::deliverContinue(detail::wiring::receiver(std::get<I>(bound_)), msg) && ...);
        }

        template<class T, class Origin, std::size_t... I>
        void publishFrom(const Origin& origin, const T& msg, std::index_sequence<I...>) const noexcept
        {
            constexpr bool unique = detail::wiring::countOf<Origin, detail::wiring::receiver_t<Bound>...> == 1;
            (detail::wiring::deliverExcept<unique>(detail::wiring::receiver(std::get<I>(bound_)), msg, origin), ...);
        }

        template<class Origin, class T, std::size_t... I>
        void publishFromType(const T& msg, std::index_sequence<I...>) const noexcept
        {
            (detail::wiring::deliverExceptType<Origin>(detail::wiring::receiver(std::get<I>(bound_)), msg), ...);
        }

        // mutable: publish() is const, and a by-value adapter must stay callable through it (a const adapter
        // whose receive() is non-const would silently stop matching the capability check)
        mutable std::tuple<detail::wiring::stored_t<Bound>...> bound_;
    };

    /** Bind receivers at the composition point: `auto bus = sub0::wire(a, b, logger); bus.publish(Sample{1});` */
    template<class... Bound>
    constexpr Wiring<Bound...> wire(Bound&... bound) noexcept { return Wiring<Bound...>(bound...); }

    /** Static topology for receivers with static storage duration: the targets are template arguments, so the
     *  wiring needs no storage: `using Bus = sub0::StaticWiring<&a, &b>; Bus::publish(Sample{1});`
     */
    template<auto*... Bound>
    struct StaticWiring
    {
        /// Deliver to every bound receiver that handles T, in bound order
        template<class T>
        static void publish(const T& msg) noexcept
        {
            (detail::wiring::deliver(detail::wiring::receiver(*Bound), msg), ...);
        }

        /// As publish(), stopping at the first receiver whose bool receive() returns false
        template<class T>
        static void publishCancelable(const T& msg) noexcept
        {
            (detail::wiring::deliverContinue(detail::wiring::receiver(*Bound), msg) && ...);
        }

        /// Ingress from one of the bound receivers: every other receiver gets it
        template<class T, class Origin>
        static void publishFrom(const Origin& origin, const T& msg) noexcept
        {
            constexpr bool unique = detail::wiring::countOf<Origin, detail::wiring::receiver_t<std::remove_pointer_t<decltype(Bound)>>...> == 1;
            (detail::wiring::deliverExcept<unique>(detail::wiring::receiver(*Bound), msg, origin), ...);
        }

        /// Ingress identified by the origin's type, which must be bound exactly once
        template<class Origin, class T>
        static void publishFrom(const T& msg) noexcept
        {
            static_assert(detail::wiring::countOf<Origin, detail::wiring::receiver_t<std::remove_pointer_t<decltype(Bound)>>...> == 1,
                          "publishFrom<Origin>: Origin must be bound exactly once; pass the origin object instead");
            (detail::wiring::deliverExceptType<Origin>(detail::wiring::receiver(*Bound), msg), ...);
        }
    };

    /** A type-erased publication port for one message type, for publishers that are not templates (a library or
     *  translation-unit boundary). One indirect call reaches the typed wiring; everything behind it stays static.
     */
    template<class T>
    class Sink
    {
    public:
        /// Wrap a wiring, which must outlive the Sink (constrained: copying a Sink copies it, never wraps it)
        template<class W, std::enable_if_t<!std::is_same_v<std::remove_cv_t<W>, Sink>, int> = 0>
        explicit Sink(W& wiring) noexcept
            : target_(&wiring)
            , call_([](const void* w, const T& msg) noexcept { static_cast<const W*>(w)->publish(msg); })
        {}

        void publish(const T& msg) const noexcept { call_(target_, msg); }

    private:
        const void* target_;
        void (*call_)(const void*, const T&) noexcept;
    };

    /** CRTP publisher mixin for dynamic topology: holds the wiring by value and gives the derived publisher
     *  `this->publish(msg)`: `struct Sensor : sub0::Publisher<Sensor, Bus> { using Publisher::Publisher; ... };`
     */
    template<class Derived, class Out>
    class Publisher
    {
    public:
        constexpr explicit Publisher(const Out& out) noexcept : out_(out) {}

    protected:
        template<class T>
        void publish(const T& msg) const noexcept { out_.publish(msg); }

    private:
        Out out_; // by value: a Wiring is a tuple of receiver references (one hop), a StaticWiring is empty
    };

    /** Transport endpoint binding: forwards every message type the transport can send.
     *  Transport concept: send(const T&) for each message type it carries (result handling is the transport's).
     *  Ingress from the transport: wiring.publishFrom(transport, msg) skips this binding (split horizon).
     */
    template<class Transport>
    class Forward
    {
    public:
        using sub0_by_value = void;       // refers to the transport only: a Wiring holds it by value (one hop)
        using sub0_endpoint = Transport;  // split horizon: ingress may name this adapter or the transport itself

        constexpr explicit Forward(Transport& transport) noexcept : transport_(transport) {}

        /// Split-horizon identity: the transport it forwards to
        constexpr const void* sub0_identity() const noexcept { return &transport_; }

        template<class T>
        auto receive(const T& msg) const noexcept -> decltype(std::declval<Transport&>().send(msg), void())
        {
            transport_.send(msg);
        }

    private:
        Transport& transport_;
    };

    /** Transport endpoint binding for a transport with static storage: no RAM, fixed target */
    template<auto* TransportObject>
    struct StaticForward
    {
        /// Split horizon: ingress may name this binding or the transport itself as its origin
        using sub0_endpoint = detail::wiring::receiver_t<std::remove_pointer_t<decltype(TransportObject)>>;
        constexpr const void* sub0_identity() const noexcept { return &detail::wiring::receiver(*TransportObject); }

        template<class T>
        auto receive(const T& msg) noexcept -> decltype(detail::wiring::receiver(*TransportObject).send(msg), void())
        {
            detail::wiring::receiver(*TransportObject).send(msg);
        }
    };

    /** Runtime subscribers behind a static wiring: bind the port like any receiver; receivers come and go at runtime.
     *  A fixed slot array with no policy: no filter, no publish context, no locking. Delivery is in add() order.
     *  For policy on the dynamic side (capacity, filter, locking, domains, teardown contract) use BrokerPort<T>.
     *  @warning Not thread-safe: add(), remove() and publishing must not run concurrently.
     */
    template<class T, uint32_t N = 8>
    class DynamicPort
    {
    public:
        /// Implemented by runtime receivers
        struct Receiver
        {
            virtual void receive(const T&) noexcept = 0;
        protected:
            ~Receiver() = default;
        };

        void add(Receiver* r) noexcept { (void)tryAdd(r); }

        /// As add(), but reports whether it fit (capacity exceeded is otherwise silent)
        bool tryAdd(Receiver* r) noexcept
        {
            if (count_ >= N)
                return false;
            entries_[count_++] = r;
            return true;
        }

        /// Remove r, keeping the order of the others
        void remove(Receiver* r) noexcept
        {
            for (uint32_t i = 0; i < count_; ++i)
                if (entries_[i] == r)
                {
                    for (uint32_t j = i + 1; j < count_; ++j)
                        entries_[j - 1] = entries_[j];
                    --count_;
                    return;
                }
        }

        void receive(const T& msg) const noexcept { for (uint32_t i = 0; i < count_; ++i) entries_[i]->receive(msg); }

    private:
        Receiver* entries_[N] = {};
        uint32_t count_ = 0;
    };

    /** Runtime subscribers behind a static wiring, with the full per-type broker (Section 2 policy) on the dynamic
     *  side: bind the port like any receiver; Subscribe<T> objects receive through it.
     */
    template<class T>
    class BrokerPort : public Publish<T>
    {
    public:
        template<class C = config_t<T>, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        BrokerPort() noexcept {}

        template<class C = config_t<T>, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        explicit BrokerPort(Domain<T>& domain) noexcept : Publish<T>(domain) {}

        void receive(const T& msg) noexcept { this->publish(msg); }
    };


// ============================================================================
// Section 5: IPC API — Serialization, forwarding, stream protocol
// ============================================================================

    /** Interface for data provider to indicate destination buffer status
     * @see ForwardPublish
     */
    class IPublish
    {
    public:

        /** Publish the data owned by the object
         */
        virtual void publish() = 0;
    };

    template< typename Prefix_t
            , typename Header_t
            , typename Postfix_t >
    class BinaryWriter
    {
    public:
        using Config = detail::Empty; //< Not configurable by default

    public:
        /** Output header and pay-load for data as binary
         * @param stream  Stream to write into
         * @param data  Data to construct a header record and data payload for
         */
        template<typename Data_t>
        inline bool write(OStream& stream, const Data_t& data) const
        {
            return utility::write<Prefix_t>(stream)
                && utility::write(stream, Header_t(data))
                && utility::write(stream, data)
                && utility::write<Postfix_t>(stream);
        }

        bool open(OStream& stream)
        {
            /* Do nothing */
            return true;
        }

        void close( OStream& stream  )
        {
            /* Do nothing */
        }

    };

    struct Buffer
    {
        IPublish* publisher; ///< Type specific publish of buffer
        char* buffer; ///< Data buffer @note a nullptr buffer may be set for unsupported payloads where paddingSize != 0 is required
        uint_least16_t bufferSize; ///< size of buffer
        int32_t paddingSize; /**< size of buffer padding data to ignore after buffer
                                  * @note Negative pad leaves unopulated bytes in buffer which are zeroed
                                  * @note For protocol version compatibility when payloads grow
                                  */
    };

    /** @tparam  cMaxDataBufferCount  Defines the maximum number of Data type buffers the deserializer can store
    */
    template< typename Header_t, uint_fast16_t cMaxDataBufferCount = 64U >
    class BufferRegister
    {
        typedef std::pair<Header_t,Buffer> HeaderToBuffer;
        typedef std::array<HeaderToBuffer, cMaxDataBufferCount> HeaderToBufferLookup;

    public:
        BufferRegister()
            : registry_()
            , registryEnd_(registry_.begin())
        {}

        /** Register a sink to the specified typed Data buffer
         * @remark Performs insertion sorting on buffers by the IPublish::typeId() for the buffer
         * @todo Make search meahcnism selectable i.e. Array-index, hash, or binary-lookup etc
         * @remark Called by sub0::ForwardPublish<Data>
         *
         * @param[in] publisher  Buffer handling object to store and signal data completion
         * @param[in] paddingSize  Number of trailing bytes after sizeof(Data) has been consumed to ignore/discard 
         *                         for alignment or protocol-version compatibility
         */
        template < typename Data >
        void set(Data& buffer, IPublish& publisher, const int32_t paddingSize = 0U )
        {
            set( Header_t(buffer)
               , Buffer{
                     &publisher 
                    , reinterpret_cast<char*>(&buffer)
                    , static_cast<uint_least16_t>(sizeof(buffer))
                    , paddingSize
               } );
        }

        void set(const Header_t& header, const Buffer& buffer)
        {
            /// @todo make this a linked list to remove capacity limitations?
#if SUB0PUB_ASSERT
            assert(registryEnd_ < std::end(registry_)); //< Capacity reached
#endif

            typename HeaderToBufferLookup::iterator iInsert = std::lower_bound(std::begin(registry_), registryEnd_, header,
                [](const HeaderToBuffer& lhs, const Header_t& rhs) { return lhs.first < rhs; });

            const bool exists = (iInsert != registryEnd_) && (iInsert->first == header);
            if (!exists) //< Insert new entry at location
            {
                std::move_backward(iInsert, registryEnd_, registryEnd_ + 1U);
                ++registryEnd_;
                iInsert->first = header;
            }

            iInsert->second = buffer;

            if ( buffer.paddingSize < 0 ) //< Nullify unpopulated bytess
            {
                char* bufferEnd = buffer.buffer + buffer.bufferSize;
                std::fill(bufferEnd + buffer.paddingSize, bufferEnd, 0x00); //< Clear content that will not be written
            }
        }

        Buffer find(const Header_t header)
        {
            typename HeaderToBufferLookup::iterator iFind = std::lower_bound(std::begin(registry_), registryEnd_, HeaderToBuffer(header, Buffer())
                , [](const HeaderToBuffer& lhs, const HeaderToBuffer& rhs) { return lhs.first < rhs.first; });

            if ((iFind != registryEnd_) && (iFind->first == header))
                return iFind->second;
            else
                return { nullptr, nullptr, 0U , 0U };
        }

        /** Default validation check against provided header
         * @note No validation occurs by default and processing is pushed onto find() to perform respective lookup operation
         * @todo Unify find/validate so that find returns a handle that can be validated or buffer accessed etc i.e. Iterator or the likes!
         * @param header Header data to validate against
         * @return True always
        */
        bool validate(const Header_t& header) const
        {
            return true;
        }

        bool close()///< @TODO This is here as a use-case contained stream state within the buffer map! Remove/deprecate this when/as possible
        {
            /** Do nothing - no state to clear */
            return true;
        }

    private:
        HeaderToBufferLookup registry_;
        typename HeaderToBufferLookup::iterator registryEnd_; ///< Iterator to end of registry_ @note Count = registryEnd_-registry_
    };

    template< typename Prefix_t, typename Header_t, typename Postfix_t, typename BufferRegister = BufferRegister<Header_t> >
    class BinaryReader
    {
    public:
        using Config = detail::Empty; //< Not configurable by default

        enum class State { 
              Prefix///< [optional] Prefix-Delimiter is being read
            , Header ///< Data-Header  is being read
            , Data ///< Data payload is being  read
            , Postfix ///< [optional] Postfix-Delimiter is being read

            , SyncLost ///< Error state entered when an error occurs in any state i.e. Corrupted input stream

            , COUNT_ 
        };

    public:
        BinaryReader()
            : dataBufferRegistry_()
            , currentBuffer_()
            , state_()
            , prefix_()
            , header_()
            , postfix_()
        {}

        /** Initialise from IStream
        */
        bool open(IStream& stream)
        {
            //TODO: Do this on open or close?
            state_ = !std::is_void<Prefix_t>::value ? State::Prefix : stateAfter(State::Prefix);
            currentBuffer_ = findStateBuffer(state_);
            return true;
        }

        /** Read from the stream
        */
        bool update(IStream& stream)
        {
            for (;;)
            {
                // Handle SyncLost: scan for next valid prefix
                if (state_ == State::SyncLost)
                {
                    if (!tryResync(stream))
                        return false;
                }

                // Handle skip of unknown payload (data + postfix bytes)
                if (skipRemaining_ > 0)
                {
                    char skipBuf[256];
                    const auto toSkip = std::min(static_cast<uint32_t>(sizeof(skipBuf)), skipRemaining_);
#if SUB0PUB_STD
                    const auto skipped = static_cast<uint32_t>(stream.read(skipBuf, toSkip).gcount());
#else
                    const auto skipped = static_cast<uint32_t>(stream.read(skipBuf, toSkip));
#endif
                    skipRemaining_ -= skipped;
                    if (skipRemaining_ > 0)
                        return false;
                    // Skip complete — reset to next prefix
                    state_ = !std::is_void_v<Prefix_t> ? State::Prefix : stateAfter(State::Prefix);
                    currentBuffer_ = findStateBuffer(state_);
                    continue;
                }

                // Normal read
                if (!readBuffer(stream))
                    return false;
                if (state_ == State::Header)
                    return true;
            }
        }

        template < typename Data >
        void setDataPublisher(Data& dataBuffer, IPublish& publisher)
        {
#if SUB0PUB_ASSERT
            assert(!currentBuffer_.buffer); /// @todo We don't intend to support adding buffers while stream is being processed?
#endif
            dataBufferRegistry_.set(dataBuffer, publisher);
        }

        bool close( IStream& stream  )
        {
            dataBufferRegistry_.close(); ///< @TODO This is here as a use-case contained stream state wihin the buffer map! Remove/deprecate this when/as possible
            return true;
        }

    private:

        /** Returns/finds buffer for state
        */
        Buffer findStateBuffer(const State state)
        {
            switch (state)
            {
            default: //< @todo unreachable unless SyncLost
            case State::Prefix: 
                return {nullptr, reinterpret_cast<char*>(&prefix_), static_cast<uint_least16_t>( !std::is_void<Prefix_t>::value ? sizeof(prefix_) : 0U), 0U};
            case State::Header: 
                return {nullptr, reinterpret_cast<char*>(&header_), static_cast<uint_least16_t>(sizeof(header_)), 0U };
            case State::Data:   
                return dataBufferRegistry_.find(header_);
            case State::Postfix: 
                return {currentBuffer_.publisher , reinterpret_cast<char*>(&postfix_), static_cast<uint_least16_t>( !std::is_void<Postfix_t>::value ? sizeof(postfix_) : 0U), 0U};
            }
        }
        
        /** Read payload data from stream and detect payload completion
         * @return True when data packet(s) have been published, false if no completed packet was present in stream
        */
        bool readBuffer(IStream& stream)
        {
            if (currentBuffer_.bufferSize > 0)
            {
#if SUB0PUB_STD
                const uint_fast16_t readCount = static_cast<uint_fast16_t>(stream.read(currentBuffer_.buffer, currentBuffer_.bufferSize).gcount()); ///< @todo readsome() for async
#else
                const uint_fast16_t readCount = stream.read(currentBuffer_.buffer, currentBuffer_.bufferSize);
#endif
                currentBuffer_.buffer += readCount;
                currentBuffer_.bufferSize -= readCount;

                /// If buffer not complete then we need to return and await more data
                if (currentBuffer_.bufferSize > 0)
                    return false;
            }

            if (currentBuffer_.paddingSize > 0)
            {
                char ignoreBuff[256];
                const size_t ignoreSize = std::min(std::size(ignoreBuff), static_cast<size_t>(currentBuffer_.paddingSize));
    #if SUB0PUB_STD
                const uint_fast16_t ignoreCount = static_cast<uint_fast16_t>(stream.read(ignoreBuff, ignoreSize).gcount());
    #else
                const uint_fast16_t ignoreCount = stream.read(ignoreBuff, ignoreSize);
    #endif

                currentBuffer_.paddingSize -= ignoreCount;

                /// If padding not complete then we need to return and await more data
                /// @todo We could publish the data before completion of the padding... however we cannot check for a post-fix delimiter without doing pad first!?
                if (currentBuffer_.paddingSize > 0)
                    return false;
            }

            //If we got here the buffer and any padding has been read from the stream
            return stateComplete();
        }

        constexpr bool getStateStatus(const State state) const
        {
            switch (state)
            {
            default:
            case State::Prefix:
                if constexpr (!std::is_void_v<Prefix_t>)
                {
                    const Prefix_t emptyPrefix{}; // Named: taking the address of a temporary is ill-formed (GCC hard error)
                    return std::memcmp(&prefix_, &emptyPrefix, sizeof(Prefix_t)) == 0;
                }
                else
                    return true;
            case State::Header:  return dataBufferRegistry_.validate(header_);
            case State::Data:    return true;
            case State::Postfix: return postfix_ == Postfix_t();
            }
        }

        constexpr bool isPublishReady(const State currentState) const
        {
            // @todo In absence of Postfix we should probably wait for Prefix instead of just Data completion?
            return currentState == (!std::is_void<Postfix_t>::value ? State::Postfix : State::Data);
        }

        constexpr State stateAfter(const State currentState ) const
        {
            switch (currentState)
            {
            default: //< @todo unreachable
            case State::Prefix:  return State::Header;
            case State::Header:  return State::Data;
            case State::Data:    return !std::is_void<Postfix_t>::value ? State::Postfix : stateAfter(State::Postfix); ///< @note may not have Prefix_t or Postfix_t
            case State::Postfix: return !std::is_void<Prefix_t>::value ? State::Prefix : stateAfter(State::Prefix);
            }
        }

        bool checkStatusOfState(const State currentState) const
        {
            const bool stateStatus = getStateStatus(currentState);
            if(stateStatus)
                return true;

            const char* failureMessage = nullptr;
            switch(currentState)
            {
                case State::Header: failureMessage = "Binary-Header mismatch - stream corruption or incompatible data-stream"; break;
                case State::Postfix: failureMessage = "Binary-Postfix mismatch - stream corruption or incompatible data-stream"; break;
                default: failureMessage = "Sync-Lost - TODO Details"; break;
            }

            if(failureMessage != nullptr)
            {
#if __cpp_exceptions
                throw std::runtime_error(failureMessage);
#elif SUB0PUB_ASSERT
                assert((void*)0 == failureMessage);
#endif
            }

            return false;
        }

        bool stateComplete()
        {
            if( !checkStatusOfState(state_) )
            {
                // Prefix or postfix mismatch — enter SyncLost to scan for next valid frame
                state_ = State::SyncLost;
                return false;
            }

            if ( isPublishReady(state_) )
            {
                if (currentBuffer_.publisher)
                    currentBuffer_.publisher->publish();
            }

            state_ = stateAfter( state_ );
            currentBuffer_ = findStateBuffer(state_);

            // Unknown typeId: skip the payload + postfix bytes and continue to next frame
            if (currentBuffer_.buffer == nullptr && state_ == State::Data)
            {
                skipRemaining_ = header_.dataBytes;
                if constexpr (!std::is_void_v<Postfix_t>)
                    skipRemaining_ += sizeof(Postfix_t);
                return true;
            }

            if (currentBuffer_.paddingSize < 0)
            {
                currentBuffer_.bufferSize += currentBuffer_.paddingSize;
                currentBuffer_.paddingSize = 0;
            }

            return currentBuffer_.buffer != nullptr;
        }

        /** Attempt to recover from SyncLost by scanning for the next valid prefix magic
         * @return True if magic found and state reset to Header, false if more data needed
         */
        bool tryResync(IStream& stream)
        {
            if constexpr (std::is_void_v<Prefix_t>)
            {
                // No prefix defined — cannot resync
                return false;
            }
            else
            {
                // Scan one byte at a time looking for the prefix magic
                char byte;
                auto* prefixBytes = reinterpret_cast<char*>(&prefix_);
                const auto prefixSize = sizeof(Prefix_t);
                const Prefix_t expected{};

#if SUB0PUB_STD
                const auto readCount = static_cast<uint_fast16_t>(stream.read(&byte, 1).gcount());
#else
                const auto readCount = stream.read(&byte, 1);
#endif
                if (readCount == 0)
                    return false;

                // Shift prefix buffer left and append new byte
                std::memmove(prefixBytes, prefixBytes + 1, prefixSize - 1);
                prefixBytes[prefixSize - 1] = byte;

                // Check if we've found the magic
                if (std::memcmp(&prefix_, &expected, prefixSize) == 0)
                {
                    state_ = State::Header;
                    currentBuffer_ = findStateBuffer(state_);
                    return true;
                }
                return false;
            }
        }

    private:
        BufferRegister dataBufferRegistry_;
        Buffer currentBuffer_; ///< Current prefix/header/payload/postfix buffer
        State state_; ///< Which buffer is being read
        uint32_t skipRemaining_ = 0; ///< Bytes remaining to skip for unknown typeId payloads

        using MemberPrefix_t = std::conditional_t<std::is_void_v<Prefix_t>, char, Prefix_t>;
        using MemberPostfix_t = std::conditional_t<std::is_void_v<Postfix_t>, char, Postfix_t>;

        MemberPrefix_t prefix_;
        Header_t header_; ///< Packet head buffer
        MemberPostfix_t postfix_;
    };

    /** Binary protocol for serialised signal and data transfer
     * @remark The protocol consists of a Header chunk followed by Header::dataBytes bytes of payload data
     */
    struct DefaultSerialisation
    {
        struct Prefix
        {
            const uint32_t magic = sub0::utility::FourCC<'S', 'U', 'B', '0'>::value; //< Magic number to identify Sub0 network protocol packets
        };

        /** Header containing signal type information
        */
        struct Header
        {
            uint32_t typeId; ///< Data type identifier @note The Id may be user specified for inter-process
            uint32_t dataBytes; ///< Count of bytes that follow after the header data

            Header() = default;

            /** header for specified Data type
            */
            template<typename Data>
            Header( const Data& data )
#if SUB0PUB_TYPEIDNAME
                : typeId(detail::TypeInfo<Data>::typeId)
#else
                : typeId(utility::typeHash<Data>())
#endif
                , dataBytes(sizeof(Data))
            {}

            /** Sort by typeId only
            */
            bool operator < (const Header& rhs) const
            { return typeId < rhs.typeId; }

            /** Compare full equality 
            */
            bool operator == (const Header& rhs) const
            { return (typeId == rhs.typeId) && (dataBytes == rhs.dataBytes); }
        };

        struct Postfix
        {
            uint8_t delim = '\n';
            bool operator==(const Postfix& rhs) const { return delim == rhs.delim; }
        };

        using Writer = BinaryWriter<Prefix, Header, Postfix>;
        using Reader = BinaryReader<Prefix, Header, Postfix>;
    };

    /** Serialises Sub0Pub data into a target stream object
     * @remark Serialised data can be received and published using the counterpart StreamDeserializer instance
     * @remark Can be used to create inter-process transfers very easily using the specified Protocol @see sub0::DefaultSerialisation
     * @tparam  Protocol  Stream data protocol to use defining how the data header and payload is structured
     */
    template< typename Protocol = DefaultSerialisation, typename ProtocolWriter = typename Protocol::Writer >
    class StreamSerializer
    {
    public:

        using WriterConfig = typename ProtocolWriter::Config;

        using ForwardReceiver = StreamSerializer<Protocol,ProtocolWriter>; //<@note Allow disambiguation for forwarding from derived classes

    public:
        /** Construct from stream
         * @param[in] stream  Stream reference stored and used to write serialised data into
         */
        StreamSerializer( OStream& stream )
            : ostream_(stream)
            , writer_()
        {}

        bool configure( const WriterConfig& config )
        {
            if constexpr (!std::is_same_v<WriterConfig, detail::Empty>)
                return writer_.configure(ostream_, config);
            else
                return true;
        }

        /** Receives forwarded data from a subscriber and serialises it to the output stream
         * @param[in] data  Forwarded data
         */
        template<typename Data>
        void receive( const Data& data )
        {
            writer_.write( ostream_, data );
        }

        bool open()
        {
            return writer_.open(ostream_);
        }

        bool update()
        {
            return writer_.update(ostream_);
        }

        /** Reset writer internal  state
        */
        bool close()
        {
            writer_.close( ostream_ );
            ostream_.flush();
            return true;
        }

    protected:
        OStream& ostream_; ///< Stream into which data is serialised
        ProtocolWriter writer_;
    };


    /** Publishes messages from a serialised-input stream using the specified Protocol 
     * @remark StreamDeserializer can be used for inter-process or distributed systems over a network where the stream
     *  could be a TcpStream or could be a file in simple cases. The serialised data is expected to be generated from a
     *  corresponding StreamSerializer instance for the same Protocol.
     * @tparam  Protocol  Stream data protocol to use defining how the data header and payload is structured
     */
    template< typename Protocol = DefaultSerialisation, typename ProtocolReader = typename Protocol::Reader >
    class StreamDeserializer
    {
    public:

        using ReaderConfig = typename ProtocolReader::Config;

    public:
        /** Store reference to supplied IStream which will be read on update()
        */
        StreamDeserializer( IStream& istream )
            : istream_(istream)
            , reader_()
        {}

        bool configure(const ReaderConfig& config)
        {
            if constexpr (!std::is_same_v<ReaderConfig, detail::Empty>)
                return reader_.configure(istream_, config);
            else
                return true;
        }

        template < typename Data >
        void setDataPublisher( Data& dataBuffer, IPublish& publisher )
        {
            reader_.setDataPublisher(dataBuffer, publisher );
        }

        /** Prime reader internal  state
        */
        bool open()
        {
            return reader_.open(istream_);
        }

        /** Polls data from the input istream
         * @return True when data packet(s) have been published, false if no completed packet was present in istream
         */
        bool update()
        {
            return reader_.update(istream_);
        }

        /** Reset reader internal  state
        */
        bool close()
        {
            return reader_.close( istream_ );
        }

    protected:
        IStream& istream_; ///< Stream from which data is de-serialized
        ProtocolReader reader_;
    };

    /** Check for `Target::ForwardReceiver` for SFINAE 
    */
    template<typename Target>
    using forward_receiver_t = typename Target::ForwardReceiver;

    /** Forward receive() to  Target type convertible from this
     * @remark The call is made with Data type allowing for templated receive<>() handler functions @see class StreamSerializer
     * @note This uses the CRTP(curiously recurring template pattern) to forward to a target type derived from ForwardSubscribe<..>
     * @tparam  Data  Data type which will be forwarded to the derived Target implementation
     * @tparam  Target  Type of derived class which implements a function of type Target::receive<>( const Data& data ) via base inheritance or direct member
     */
    template<typename Data, typename Target >
    class ForwardSubscribe : public Subscribe<Data>
    {
    public:
        /** Receives subscribed data and forward to target object
         * @param data  Data to forward
         */
        inline void receive( const Data& data ) noexcept override
        {
            using ForwardReceiver_t = utility::detected_or_t<Target, forward_receiver_t, Target>;
            static_cast<Target*>(this)->ForwardReceiver_t::receive(data);
        }
    };

    /** Register publication of data with a provider instance
     * @remark The call is made with Data type allowing for templated receive<>() handler functions @see class StreamSerializer
     * @note This uses the CRTP(curiously recurring template pattern) to forward to a target type derived from ForwardPublish<..>
     * @tparam  Data  Data type which will be read into from a DataProvider
     * @tparam  DataProvider  CRTP Type of derived class which implements a function of type DataProvider::setDataPublisher( Data&, IPublish& ) via base inheritance or direct member
     *
     * @todo API not final
     */
    template<typename Data, typename DataProvider >
    class ForwardPublish : public Publish<Data>, protected IPublish
    {
    public:
        /** Register publisher buffer with the data provider
         * @param typeName  Unique name given to the serialised data entry @note Replaces compiler generated name which is not portable
         */
        ForwardPublish(
#if SUB0PUB_TYPEIDNAME            
            const uint32_t typeId = 0, const char* typeName = 0/*nullptr*/ 
#endif
        )
            : Publish<Data>(
#if SUB0PUB_TYPEIDNAME
                typeId, typeName
#endif
              )
            , IPublish()
        {
            DataProvider& provider = static_cast<DataProvider&>(*this);
            provider.setDataPublisher( buffer_, static_cast<IPublish&>(*this) ); // Register the buffer sink to the data provider
        }

    private:

        /** Publish the data populated in buffer_
         */
        virtual void publish() final
        { Publish<Data>::publish( buffer_ ); }

    private:
        Data buffer_ = {}; ///< Data buffer to be published 
                      ///< @todo Double-buffer data storage for asynchronous processing and receive?
    };

    /** Forward receive() to Target type convertible from this for all Datas types listed
     * @remark The call is made with Data type allowing for templated receive<>() handler functions @see `class StreamSerializer` for example `template receive<>()`
     * @note This uses the CRTP(curiously recurring template pattern) to forward to a target type derived from ForwardSubscribe<..>
     * @tparam  Data  Data type which will be forwarded to the derived Target implementation
     * @tparam  Target  Type of derived class which implements a function of type Target::receive<>( const Data& data ) via base inheritance or direct member
     */
    template< typename SubscriberTarget, typename... Datas >
    class ForwardSubscribeAll : public ForwardSubscribe<Datas, SubscriberTarget>... {};

    template<typename SubscriberTarget, typename... Datas>
    class ForwardSubscribeAll<SubscriberTarget, std::tuple<Datas...> > : public ForwardSubscribe<Datas, SubscriberTarget>... {};


    /** Register publication of data with a provider instance
    */
    template< typename DataProvider, typename... Datas >
    class ForwardPublishAll : public ForwardPublish<Datas, DataProvider>... {};

    template<typename DataProvider, typename... Datas>
    class ForwardPublishAll<DataProvider, std::tuple<Datas...> > : public ForwardPublish<Datas, DataProvider>... {};


} // END: sub0

/** Configure a Data type you cannot modify (resolution step 1c). Use at global namespace scope, next to the type:
 *  `SUB0PUB_CONFIGURE(int, sub0::Capacity<32>);`
 */
#define SUB0PUB_CONFIGURE(Type, ...) \
    template<> struct sub0::configure<Type> { using type = sub0::config<__VA_ARGS__>; }

#endif
