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

    /// Builtin defaults: exactly today's behaviour, derived from the legacy SUB0PUB_* macros
    struct Builtin
    {
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

        /// Subscription table for one Data type (Global) or one Domain (Scoped). Lock is an empty base when NoLock.
        template<class Data, class Config>
        struct Table : Config::Lock
        {
            uint32_t count = 0;
            Subscribe<Data>* entries[Config::capacity] = {};
        };

        /** Publish context used by cancel() and re-entrancy checks, per Context policy
         * @remark `current` identifies the subscription table being dispatched (one per Data type for Global storage,
         *         one per Domain for Scoped), so cancel() and DirectChecked only act on their own table's dispatch.
         */
        template<class Data, Context C> struct PublishContext
        {
            static constexpr bool enabled = false;
        };
        template<class Data> struct PublishContext<Data, Context::ThreadLocal>
        {
            static constexpr bool enabled = true;
            inline static thread_local const void* current = nullptr;
            inline static thread_local bool canceled = false;
        };
        template<class Data> struct PublishContext<Data, Context::Static>
        {
            static constexpr bool enabled = true;
            inline static const void* current = nullptr;
            inline static bool canceled = false;
        };

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

        /** Broker for one Data type with one resolved configuration
         * @remark Every TU must resolve the same configuration for a Data type (build contract, see Registry).
         */
        template<class Data, class Config>
        class Broker
        {
            static_assert(Config::capacity > 0, "sub0x: Capacity must be at least 1");
            static_assert(!(Config::dispatch == Dispatch::DirectChecked && Config::context == Context::None),
                          "sub0x: DirectChecked needs a publish context (ThreadLocalContext or StaticContext)");
            static_assert(std::is_same_v<typename Config::Lock, NoLock> || Config::dispatch == Dispatch::Snapshot,
                          "sub0x: a Lock requires Snapshot dispatch (receivers are called outside the lock)");

            using Ctx = PublishContext<Data, Config::context>;
            using TableT = Table<Data, Config>;
            static constexpr bool cScoped = Config::storage == Storage::Scoped;

        public:
            using Configuration = Config;

            Broker() noexcept : scope_() { checkConfig<Data, Config>(); }
            explicit Broker(TableT& table) noexcept : scope_(table) { checkConfig<Data, Config>(); }

            sub0::SubscribeResult trySubscribe(Subscribe<Data>* subscriber) noexcept
            {
                TableT& t = table();
                LockGuard<Config> lk(t);
                checkNotDispatching();
                if (t.count >= Config::capacity)
                    return sub0::SubscribeResult::CapacityExceeded;
                t.entries[t.count++] = subscriber;
                return sub0::SubscribeResult::Subscribed;
            }

            void unsubscribe(Subscribe<Data>* subscriber) noexcept
            {
                TableT& t = table();
                LockGuard<Config> lk(t);
                Subscribe<Data>** const it = std::find(t.entries, t.entries + t.count, subscriber);
                if (it == t.entries + t.count)
                    return;
                checkNotDispatching();
                std::move(it + 1, t.entries + t.count, it);
                --t.count;
            }

            void publish(const Data& data) const noexcept
            {
                TableT& t = table();
                if constexpr (Config::dispatch == Dispatch::Snapshot)
                {
                    Subscribe<Data>* snapshot[Config::capacity];
                    uint32_t count;
                    {
                        LockGuard<Config> lk(t);
                        count = t.count;
                        std::copy_n(t.entries, count, snapshot);
                    }
                    dispatch(snapshot, count, data);
                }
                else
                {
                    checkNotDispatching();
                    dispatch(t.entries, t.count, data);
                }
            }

            /// Cancel the dispatch in progress on this broker's table; a no-op for any other table's dispatch
            void cancel() const noexcept
            {
                static_assert(Ctx::enabled, "sub0x: cancel() needs a publish context (ThreadLocalContext or StaticContext)");
                if constexpr (Ctx::enabled)
                    if (Ctx::current == &table())
                        Ctx::canceled = true;
            }

        private:
            template<class Entries>
            void dispatch(Entries& entries, const uint32_t& count, const Data& data) const noexcept
            {
                if constexpr (Ctx::enabled)
                {
                    const void* const previous = Ctx::current;
                    const bool previousCanceled = Ctx::canceled;
                    Ctx::current = &table();
                    Ctx::canceled = false;
                    for (uint32_t i = 0; !Ctx::canceled && i < count; ++i)
                        deliver(entries[i], data);
                    Ctx::current = previous;
                    Ctx::canceled = previousCanceled;
                }
                else
                {
                    for (uint32_t i = 0; i < count; ++i)
                        deliver(entries[i], data);
                }
            }

            static void deliver(Subscribe<Data>* s, const Data& data) noexcept;

            /// DirectChecked: only a use of the table currently being iterated is a violation (other domains are fine)
            void checkNotDispatching() const noexcept
            {
                if constexpr (Config::dispatch == Dispatch::DirectChecked)
                    if (Ctx::current == &table())
                        SUB0X_REENTRANT_VIOLATION("sub0x: re-entrant use of a Data type's table during its own dispatch requires Snapshot");
            }

            TableT& table() const noexcept
            {
                if constexpr (cScoped)
                    return scope_;
                else
                    return global_;
            }

            struct Unscoped { Unscoped() = default; };
            std::conditional_t<cScoped, TableT&, Unscoped> scope_; ///< empty for Global storage
            inline static TableT global_ = {};

            template<class> friend class sub0x::Domain;
        };

        template<class Data>
        using BrokerFor = Broker<Data, config_t<Data>>;
    }

    /// Scope for Storage::Scoped types (issue #5): independent subscription tables for the same Data type
    template<class Data>
    class Domain
    {
        static_assert(config_t<Data>::storage == Storage::Scoped, "sub0x: Domain<Data> requires a Data type configured with sub0x::Scoped");
    public:
        Domain() = default;
        Domain(const Domain&) = delete;
        Domain& operator=(const Domain&) = delete;
    private:
        template<class> friend class Subscribe;
        template<class> friend class Publish;
        detail::Table<Data, config_t<Data>> table_;
    };

    /** Subscriber base; configuration-dependent interface (filter() only if configured) */
    template<class Data>
    class Subscribe : public detail::SubscriberInterface<Data, config_t<Data>::filter>
    {
        using Config = config_t<Data>;
        using Broker = detail::BrokerFor<Data>;
        template<class, class> friend class detail::Broker;
    public:
        template<class C = Config, std::enable_if_t<C::storage == Storage::Global, int> = 0>
        Subscribe() noexcept { trySubscribe(); }

        template<class C = Config, std::enable_if_t<C::storage == Storage::Scoped, int> = 0>
        explicit Subscribe(Domain<Data>& domain) noexcept : broker_(domain.table_) { trySubscribe(); }

        Subscribe(const Subscribe&) = delete;
        Subscribe& operator=(const Subscribe&) = delete;

        virtual ~Subscribe()
        {
            if (subscribed_)
                broker_.unsubscribe(this);
        }

        bool isSubscribed() const noexcept { return subscribed_; }

        sub0::SubscribeResult trySubscribe() noexcept
        {
            if (subscribed_)
                return sub0::SubscribeResult::Subscribed;
            const sub0::SubscribeResult result = broker_.trySubscribe(this);
            subscribed_ = (result == sub0::SubscribeResult::Subscribed);
            return result;
        }

        void cancel() const noexcept { broker_.cancel(); }

    private:
        Broker broker_;
        bool subscribed_ = false;
    };

    template<class Data, class Config>
    void detail::Broker<Data, Config>::deliver(Subscribe<Data>* s, const Data& data) noexcept
    {
        if constexpr (Config::filter)
            if (!s->filter(data))
                return;
        s->receive(data);
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
        void publish(const Data& data) const noexcept { broker_.publish(data); }

    private:
        template<class From, class D> friend void publish(From&, const D&) noexcept;
        Broker broker_;
    };

    template<class From, class Data>
    inline void publish(From& from, const Data& data) noexcept
    {
        const Publish<Data>& publisher = from;
        publisher.publish(data);
    }

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
