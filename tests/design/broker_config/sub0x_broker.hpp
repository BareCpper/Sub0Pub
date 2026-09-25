#pragma once
/** PROTOTYPE: per-Data broker configuration for Sub0Pub v2 (docs/design/BROKER_CUSTOMISATION.md)
 *
 * Experimental namespace sub0x. Not part of the public API and does not modify sub0pub.hpp; it exists
 * to prove the design compiles, is ODR-safe across translation units and costs nothing when unused.
 *
 * Resolution of the configuration used by every Subscribe<Data> / Publish<Data> / publish() of a type:
 *
 *   1. Per-Data, exactly one of:
 *      a. member alias    struct Imu { ...; using sub0_config = sub0x::config<sub0x::Capacity<2>>; };
 *      b. ADL declaration sub0x::config<...> sub0_config(Gps*);          // in Gps's namespace, declaration only
 *      c. traits          SUB0X_CONFIGURE(int, sub0x::Capacity<32>)     // types you cannot modify
 *   2. Global default: SUB0X_DEFAULT_CONFIG, from a header named by SUB0X_CONFIG_HEADER (set by the build system)
 *   3. Builtin: today's SUB0PUB_* macros
 *
 * 1a/1b are part of the type's own definition, so every TU that sees the complete type agrees by construction.
 * 1c and 2 must be visible in every TU; a debug-build registry detects a TU that resolved differently.
 */
#include <sub0pub/sub0pub.hpp> // SUB0PUB_* defaults and utility::typeHash only

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <thread>
#include <type_traits>

namespace sub0x
{
    // ========================================================================
    // Policy vocabulary
    // ========================================================================

    enum class Dispatch : uint8_t
    {
        Snapshot,       ///< Copy the table before dispatch: re-entrant publish/subscribe safe (today's default)
        Direct,         ///< Iterate the live table: fastest, no re-entrancy
        DirectChecked   ///< Direct + detect re-entrancy (needs Context != None)
    };

    enum class Context : uint8_t
    {
        ThreadLocal,    ///< thread_local publish context: cancel() + nested publish on any thread (today)
        Static,         ///< plain static context: cancel() + nesting, single thread only, no TLS required
        None            ///< no publish context: cancel() unavailable, no per-publish context cost
    };

    enum class Storage : uint8_t
    {
        Global,         ///< one table per Data type per module (today's MonoState)
        Scoped          ///< tables live in sub0x::Domain<Data> instances passed at construction (issue #5)
    };

    struct NoLock
    {
        void lock() noexcept {}
        void unlock() noexcept {}
    };

#if SUB0PUB_THREAD_SAFE
    struct StdMutexLock
    {
        void lock() noexcept { m.lock(); }
        void unlock() noexcept { m.unlock(); }
        std::mutex m;
    };
#endif

    namespace detail
    {
        template<class Data, class Config> class Broker; ///< the library broker (default implementation)
    }

    /// Builtin defaults: exactly today's behaviour, derived from the legacy SUB0PUB_* macros
    struct Builtin
    {
        /// Broker implementation (see Implementation<> and the broker concept in BROKER_CUSTOMISATION.md)
        template<class Data, class Config> using broker = detail::Broker<Data, Config>;
        static constexpr uint32_t capacity = SUB0PUB_MAX_SUBSCRIPTIONS;
        static constexpr Dispatch dispatch =
            (SUB0PUB_REENTRANT_SAFE || SUB0PUB_THREAD_SAFE) ? Dispatch::Snapshot
            : (SUB0PUB_REENTRANT_CHECK ? Dispatch::DirectChecked : Dispatch::Direct);
        static constexpr Context context = Context::ThreadLocal;
        static constexpr Storage storage = Storage::Global;
        static constexpr bool filter = true;
#if SUB0PUB_THREAD_SAFE
        using Lock = StdMutexLock;
#else
        using Lock = NoLock;
#endif
    };

    // Options: each applies itself on top of a base configuration --------------------------------

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

    template<class L> struct LockWith
    { template<class B> struct apply : B { using Lock = L; }; };

    struct NoFilter
    { template<class B> struct apply : B { static constexpr bool filter = false; }; };

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

} // namespace sub0x

// Global default (2): the project config header is included once the option vocabulary exists, so it can
// define e.g. `struct ProjectDefaults : sub0x::with<sub0x::Builtin, sub0x::NoFilter> {};` and
// `#define SUB0X_DEFAULT_CONFIG ProjectDefaults`. Set SUB0X_CONFIG_HEADER from the build system so every TU agrees.
#if defined(SUB0X_CONFIG_HEADER)
#include SUB0X_CONFIG_HEADER
#endif

#ifndef SUB0X_CHECK_CONFIG
#if SUB0PUB_ASSERT && !defined(NDEBUG)
#define SUB0X_CHECK_CONFIG true
#else
#define SUB0X_CHECK_CONFIG false
#endif
#endif

#ifndef SUB0X_CONFIG_MISMATCH
#define SUB0X_CONFIG_MISMATCH(what) do { assert(!(what)); std::abort(); } while(false)
#endif

#ifndef SUB0X_REENTRANT_VIOLATION
#define SUB0X_REENTRANT_VIOLATION(what) SUB0PUB_REENTRANT_VIOLATION(what)
#endif


namespace sub0x
{
#if defined(SUB0X_DEFAULT_CONFIG)
    using GlobalDefault = SUB0X_DEFAULT_CONFIG;
#else
    using GlobalDefault = Builtin;
#endif

    /// Per-Data configuration: the global default with options applied. The usual spelling at a Data site.
    template<class... Opts>
    struct config : with<GlobalDefault, Opts...> {};

    // ========================================================================
    // Resolution
    // ========================================================================

    /// (1c) Traits hook for types that cannot carry a member alias or ADL declaration (int, std::, third-party)
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
            static_assert(!std::is_reference_v<Data> && !std::is_const_v<Data>, "sub0x: Data must be an unqualified object type");
            static constexpr int count = int(member_config<Data>::found) + int(adl_config<Data>::found) + int(traits_config<Data>::found);
            static_assert(count <= 1, "sub0x: a Data type must be configured in exactly one place "
                                      "(member sub0_config, ADL sub0_config(Data*), or sub0x::configure<Data>)");
            using type = std::conditional_t<member_config<Data>::found, member_config<Data>,
                         std::conditional_t<adl_config<Data>::found, adl_config<Data>,
                         std::conditional_t<traits_config<Data>::found, traits_config<Data>,
                         std::enable_if<true, GlobalDefault>>>>;
        };
    }

    /// The configuration every use of Data resolves to (public for introspection and static_asserts)
    template<class Data>
    using config_t = typename detail::resolve<Data>::type::type;

    template<class Data> class Subscribe;
    template<class Data> class Publish;
    template<class Data> class Domain;

    // ========================================================================
    // Results
    // ========================================================================

    enum class SubscribeResult : uint8_t
    {
        Subscribed,        ///< registered: receives subsequent publishes
        CapacityExceeded,  ///< table full; table unchanged
        Closed             ///< the subscriber's Domain has been closed
    };

    /// Outcome of handing a message to a transport. Acceptance is NOT remote delivery.
    enum class SendResult : uint8_t
    {
        Accepted,          ///< the transport took the message (copied/serialized it)
        Full,              ///< temporary: queue/buffer exhausted
        Disconnected,      ///< no peer at the moment
        Closed             ///< the transport is shutting down / shut down
    };

    /// Opt-in per-publish report of route results (sub0x::publish(from, data, report)). Local delivery is not
    /// affected by route results: every local subscriber is still called when a route rejects.
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
        /** Fingerprint of a configuration's effective values (not its type name)
         * @remark Two differently-named configurations with the same values fingerprint equal; the same name
         *         with different macro-derived values (e.g. Builtin under different SUB0PUB_* flags) does not.
         */
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
                sub0::utility::typeHash<typename Config::Lock>()
            };
            for (uint32_t f : fields)
                h = ((h << 5) + h) ^ f;
            return h | 1U; // never 0, which marks "unregistered"
        }

        /** Best-effort debug diagnostic for inconsistent configuration visibility across TUs
         * @warning Resolving a Data type differently in two TUs is an ODR violation (Subscribe<Data>'s bases and
         *          members depend on the configuration) and therefore undefined behaviour. Consistent visibility is
         *          a build contract; this registry only reports violations it happens to observe at runtime.
         * @remark Registry<Data> has no dependency on the configuration, so it is shared by all TUs.
         */
        template<class Data>
        struct Registry
        {
            inline static std::atomic<uint32_t> fingerprint{0};
        };

        template<class Data, class Config>
        void checkConfig() noexcept
        {
#if SUB0X_CHECK_CONFIG
            constexpr uint32_t mine = configFingerprint<Config>();
            uint32_t seen = 0;
            if (!Registry<Data>::fingerprint.compare_exchange_strong(seen, mine, std::memory_order_relaxed) && seen != mine)
                SUB0X_CONFIG_MISMATCH("sub0x: Data type resolved to different configurations in different translation units");
#endif
        }

        template<class Config>
        struct LockGuard
        {
            explicit LockGuard(typename Config::Lock& l) noexcept : l_(l) { l_.lock(); }
            ~LockGuard() { l_.unlock(); }
            typename Config::Lock& l_;
        };

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

        /// Subscription table for one Data type (Global) or one Domain (Scoped). Empty bases cost nothing.
        template<class Data, class Config>
        struct Table : Config::Lock, ActiveList<Data, cConcurrent<Config>>, ScopeState<Config::storage == Storage::Scoped>
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

    // ========================================================================
    // Broker author kit: what an application-defined broker (Implementation<>) builds on
    // ========================================================================

    namespace kit
    {
        /** RAII dispatch frame: push while calling receivers so cancel(), routes (origin, report) and same-thread
         * disconnect work. Zero-size when the Data type's configuration has no publish context.
         */
        template<class Data>
        class DispatchScope
        {
            using Ctx = detail::PublishContext<Data, config_t<Data>::context>;
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

        /// Call one subscriber: filter() (when configured) then receive()
        template<class Data>
        void deliver(Subscribe<Data>* s, const Data& data) noexcept;

        /// Innermost dispatch in progress on this thread for Data, or nullptr
        template<class Data>
        const detail::Frame<Data>* activeDispatch() noexcept
        {
            using Ctx = detail::PublishContext<Data, config_t<Data>::context>;
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
            using Ctx = detail::PublishContext<Data, config_t<Data>::context>;
            static_assert(Ctx::enabled, "sub0x: cancel() needs a publish context (ThreadLocalContext or StaticContext)");
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
            using Ctx = detail::PublishContext<Data, config_t<Data>::context>;
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
            virtual void receive(const Data& data) noexcept = 0;
        protected:
            ~SubscriberInterface() = default;
        };
        template<class Data>
        class SubscriberInterface<Data, true>
        {
        public:
            virtual void receive(const Data& data) noexcept = 0;
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
         *
         * @remark Every TU must resolve the same configuration for a Data type (build contract, see Registry).
         */
        template<class Data, class Config>
        class Broker
        {
            static_assert(Config::capacity > 0, "sub0x: Capacity must be at least 1");
            static_assert(!(Config::dispatch == Dispatch::DirectChecked && Config::context == Context::None),
                          "sub0x: DirectChecked needs a publish context (ThreadLocalContext or StaticContext)");
            static_assert(!cConcurrent<Config> || Config::dispatch == Dispatch::Snapshot,
                          "sub0x: a Lock requires Snapshot dispatch (receivers are called outside the lock)");
            static_assert(!cConcurrent<Config> || Config::context != Context::None,
                          "sub0x: a Lock requires a publish context (disconnect must not wait on its own dispatch)");

            using Ctx = PublishContext<Data, Config::context>;
            static constexpr bool cScoped = Config::storage == Storage::Scoped;

        public:
            using Configuration = Config;
            using TableT = Table<Data, Config>;

            template<bool S = cScoped, std::enable_if_t<!S, int> = 0>
            Broker() noexcept { checkConfig<Data, Config>(); }

            template<bool S = cScoped, std::enable_if_t<S, int> = 0>
            explicit Broker(TableT& table) noexcept : scope_(table) { checkConfig<Data, Config>(); }

            SubscribeResult trySubscribe(Subscribe<Data>* subscriber) noexcept
            {
                TableT& t = table();
                LockGuard<Config> lk(t);
                checkNotDispatching(t);
                if constexpr (cScoped)
                    if (t.closed)
                        return SubscribeResult::Closed;
                if (t.count >= Config::capacity)
                    return SubscribeResult::CapacityExceeded;
                t.entries[t.count++] = subscriber;
                return SubscribeResult::Subscribed;
            }

            /** Remove `subscriber`; on return no dispatch (on any thread) will call it again
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
                    Subscribe<Data>** const it = std::find(t.entries, t.entries + t.count, subscriber);
                    if (it != t.entries + t.count)
                    {
                        checkNotDispatching(t);
                        std::move(it + 1, t.entries + t.count, it);
                        --t.count;
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
                            if (snapshot[i].load(std::memory_order_seq_cst) == s) // not disconnected meanwhile
                                kit::deliver(s, data);
                            active.current.store(nullptr, std::memory_order_seq_cst);
                        }
                    }
                    LockGuard<Config> lk(t);
                    t.unlink(active);
                }
                else if constexpr (Config::dispatch == Dispatch::Snapshot)
                {
                    Subscribe<Data>* snapshot[Config::capacity];
                    uint32_t count;
                    if constexpr (cScoped)
                        if (t.closed)
                            return;
                    count = t.count;
                    std::copy_n(t.entries, count, snapshot);
                    kit::DispatchScope<Data> scope(&t, origin, report, snapshot, count);
                    for (uint32_t i = 0; !scope.canceled() && i < count; ++i)
                        if (Subscribe<Data>* const s = snapshot[i])
                            kit::deliver(s, data);
                }
                else
                {
                    checkNotDispatching(t);
                    kit::DispatchScope<Data> scope(&t, origin, report, nullptr, 0);
                    for (uint32_t i = 0; !scope.canceled() && i < t.count; ++i)
                        kit::deliver(t.entries[i], data);
                }
            }

            void cancel() const noexcept { kit::cancel<Data>(&table()); }

            /// Close a Scoped table: reject subscriptions, drop publishes, detach subscribers, then quiesce
            static void close(TableT& t) noexcept;

        private:
            /// Concurrent: null `s` (or every entry when s == nullptr) in all active snapshots. Call under the table lock.
            static void forgetInActiveDispatches(TableT& t, const Subscribe<Data>* s) noexcept
            {
                for (ActiveDispatch<Data>* a = t.activeHead; a; a = a->next)
                    for (uint32_t i = 0; i < a->count; ++i)
                        if (s == nullptr || a->snapshot[i].load(std::memory_order_relaxed) == s)
                            a->snapshot[i].store(nullptr, std::memory_order_seq_cst);
            }

            /// Concurrent: wait while another thread is inside `s`'s callback (any callback when s == nullptr)
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
                        SUB0X_REENTRANT_VIOLATION("sub0x: re-entrant use of a Data type's table during its own dispatch requires Snapshot");
                (void)t;
            }

            TableT& table() const noexcept { return *scope_.get(global_); }

            ScopeRef<TableT, cScoped> scope_;
            inline static TableT global_;
        };

        template<class Data>
        using BrokerFor = typename config_t<Data>::template broker<Data, config_t<Data>>;
    }

#ifndef SUB0X_DOMAIN_LIFETIME
#define SUB0X_DOMAIN_LIFETIME(what) do { assert(!(what)); std::abort(); } while(false)
#endif

    /** Session scope for Storage::Scoped types (issue #5): independent subscription tables for the same Data type
     * @remark Lifetime contract: a Domain must outlive every Subscribe/Publish/Route bound to it (debug-checked).
     *         close() ends the session early: subscribe returns Closed, publish is dropped, current subscribers are
     *         detached, and in-flight dispatches are waited for.
     */
    template<class Data>
    class Domain
    {
        using Config = config_t<Data>;
        using BrokerT = detail::BrokerFor<Data>;
        static_assert(Config::storage == Storage::Scoped, "sub0x: Domain<Data> requires a Data type configured with sub0x::Scoped");
        static_assert(std::is_same_v<BrokerT, detail::Broker<Data, Config>>, "sub0x: Scoped storage requires the library broker (prototype)");
    public:
        Domain() = default;
        Domain(const Domain&) = delete;
        Domain& operator=(const Domain&) = delete;

        ~Domain()
        {
            close();
            if (table_.handles.load(std::memory_order_acquire) != 0)
                SUB0X_DOMAIN_LIFETIME("sub0x: Domain destroyed while Subscribe/Publish/Route handles are still bound to it");
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

    /** Subscriber base; configuration-dependent interface (filter() only if configured)
     *
     * Activation contract: single-threaded configurations register in the constructor. Concurrent configurations
     * (a Lock) do not: another thread could otherwise dispatch into the object before the derived class is
     * constructed. Call trySubscribe() at the end of the most-derived constructor (Route does this).
     *
     * Teardown contract: after disconnect() returns, receive() is not called again, on any thread. The base destructor
     * disconnects too, but by then the derived object is already destroyed: when other threads may publish, call
     * disconnect() from the most-derived destructor (Route does this). Same-thread disconnect during a dispatch,
     * including from the subscriber's own receive(), is safe with Snapshot dispatch.
     */
    template<class Data>
    class Subscribe : public detail::SubscriberInterface<Data, config_t<Data>::filter>
    {
        using Config = config_t<Data>;
        using Broker = detail::BrokerFor<Data>;
        template<class> friend class Domain;
        template<class, class> friend class detail::Broker;
    public:
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        Subscribe() noexcept { activateIfSingleThreaded(); }

        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        explicit Subscribe(Domain<Data>& domain) noexcept : broker_(domain.table_) { activateIfSingleThreaded(); }

        Subscribe(const Subscribe&) = delete;
        Subscribe& operator=(const Subscribe&) = delete;

        virtual ~Subscribe() { disconnect(); }

        bool isSubscribed() const noexcept { return subscribed_.load(std::memory_order_relaxed); }

        SubscribeResult trySubscribe() noexcept
        {
            if (isSubscribed())
                return SubscribeResult::Subscribed;
            const SubscribeResult result = broker_.trySubscribe(this);
            subscribed_.store(result == SubscribeResult::Subscribed, std::memory_order_relaxed);
            return result;
        }

        /// Stop receiving. Idempotent; safe from within receive(); see the teardown contract above
        void disconnect() noexcept
        {
            const bool wasSubscribed = subscribed_.exchange(false, std::memory_order_relaxed);
            // Concurrent: always, so a subscriber already detached by Domain::close() still waits out a callback in
            // progress on another thread. Single-threaded: close() already made it safe; nothing left to do.
            if (detail::cConcurrent<Config> || wasSubscribed)
                broker_.disconnect(this);
        }

        void cancel() const noexcept { broker_.cancel(); }

    protected:
        void activateIfSingleThreaded() noexcept
        {
            if constexpr (!detail::cConcurrent<Config>)
                trySubscribe();
        }

        /// For bindings (Route): publish into this subscriber's table with an ingress origin
        void injectFrom(const void* origin, const Data& data) const noexcept { broker_.publish(data, origin, nullptr); }

    private:
        Broker broker_;
        std::atomic<bool> subscribed_{false};
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
    }

    template<class Data, class Config>
    void detail::Broker<Data, Config>::close(TableT& t) noexcept
    {
        static_assert(cScoped, "sub0x: only Scoped tables can be closed");
        uint32_t n;
        Subscribe<Data>* detached[Config::capacity];
        {
            LockGuard<Config> lk(t);
            t.closed = true;
            n = t.count;
            std::copy_n(t.entries, n, detached);
            for (uint32_t i = 0; i < n; ++i)
                t.entries[i]->subscribed_.store(false, std::memory_order_relaxed);
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

    /** Publisher base: no virtual destructor (publishers hold no registration), so no vptr */
    template<class Data>
    class Publish
    {
        using Config = config_t<Data>;
        using Broker = detail::BrokerFor<Data>;
    public:
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        Publish() noexcept {}

        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        explicit Publish(Domain<Data>& domain) noexcept : broker_(domain.table_) {}

    protected:
        void publish(const Data& data, PublishReport* report = nullptr) const noexcept { broker_.publish(data, nullptr, report); }

    private:
        template<class From, class D> friend void publish(From&, const D&) noexcept;
        template<class From, class D> friend void publish(From&, const D&, PublishReport&) noexcept;
        Broker broker_;
    };

    template<class From, class Data>
    inline void publish(From& from, const Data& data) noexcept
    {
        const Publish<Data>& publisher = from;
        publisher.publish(data);
    }

    /// Publish and report route results (routed / accepted / rejected). Local delivery is unaffected by rejections.
    template<class From, class Data>
    inline void publish(From& from, const Data& data, PublishReport& report) noexcept
    {
        static_assert(config_t<Data>::context != Context::None, "sub0x: publish reports need a publish context");
        const Publish<Data>& publisher = from;
        publisher.publish(data, &report);
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
        static_assert(Config::context != Context::None, "sub0x: Route needs a publish context (split horizon and reports)");
    public:
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        explicit Route(Transport& transport) noexcept : transport_(transport) { this->trySubscribe(); }

        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        Route(Domain<Data>& domain, Transport& transport) noexcept : Subscribe<Data>(domain), transport_(transport) { this->trySubscribe(); }

        ~Route() override { this->disconnect(); }

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

    // Tagged types (docs/TAGGED_TYPES_PROPOSAL.md) take their configuration from the tag, so fundamental
    // and third-party payloads can be configured without touching the payload type.
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

} // namespace sub0x

/// (1c) Configure a type you cannot modify. Use at global namespace scope, next to the type's declaration.
#define SUB0X_CONFIGURE(Type, ...) \
    template<> struct sub0x::configure<Type> { using type = sub0x::config<__VA_ARGS__>; }
