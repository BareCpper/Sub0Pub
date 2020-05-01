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
#include <cassert> //< assert
#include <cstring> //< std::strcmp
#include <array> //< std::array @todo Should we not use this one occurrence for C++98 compatibility?
//#include <typeinfo> //< typeid()
#include <type_traits> //< std::is_same

 /// @todo 0 vs nullptr C++11 only
#if 1 /// @todo cstdint not always available ... C++11/C99 only 
    #include <cstdint> //< uint32_t
#else
    typedef unsigned char uint8_t;
    typedef unsigned int uint32_t;
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
#define SUB0PUB_TYPEIDNAME false ///< Typs iven unitq/user-defined type index and strng name for diagnostis and IPC
#endif

/** Helper macro for stringifying value using compiler preprocessor
 * e.g. SUB0_STRINGIFY_HELPER(123) == "123", SUB0_STRINGIFY_HELPER(FooBar) == "FooBar"
 * @param  x  A value whos value will be converted to string e.g. FooBar == "FooBar", 123 = "123"
 */
#define SUB0_STRINGIFY_HELPER(x) #x

/** Helper macro for stringifying define using compiler preprocessor
 * e.g. SUB0_STRINGIFY_HELPER(__LINE__) == "123??"
 * @param  x  A macro definition whos value will be converted to string  e.g. __LINE__ == "123??"
 */
#define SUB0_STRINGIFY(x) SUB0_STRINGIFY_HELPER(x)

#if SUB0PUB_STD
#include <ostream> //< std::ostream
#include <istream> //< std::istream
#endif

/// @todo Trace interface - currently std::cout only!!
#if SUB0PUB_TRACE
#include <iostream>
#endif

/** Sub0Pub top-level namespace
*/
namespace sub0
{
    /** Internal utility functions
    */
    namespace utility
    {
        /** Create 4byte packed value at compile time
         * @tparam a,b,c,d  Characters which will be packed into 4-byte uint32_t value
         */
        template <const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d>
        struct FourCC
        {
            static const uint32_t value = (((((d << 8) | c) << 8) | b) << 8) | a;
        };

        /** Hash a string using djb2 hash
         * @param[in] str  Null-terminated string to calculate hash of
         * @todo Implement as compile time with name
         * @return djb2 hash value for input 'str'
         */
        inline uint32_t hash( const char* str)
        {
            uint32_t hash = 5381U;
            for ( ; str[0U] != '\0'; ++str )
            {
                hash = ((hash << 5) + hash) + str[0U]; /* hash * 33 + c */
            }
            return hash;
        }

        class OStream
        {
        public:
            typedef uint_fast16_t StreamSize;

            virtual StreamSize write(const char* const buffer, const StreamSize bufferCount ) = 0;
        };

        class IStream
        {
        public:
            typedef uint_fast16_t StreamSize;

            virtual StreamSize read( char* const buffer, const StreamSize bufferCount ) = 0;
        };

        template< typename Type_t >
        inline void write(OStream& stream)
        {
            const Type_t defaulted;
            stream.write(reinterpret_cast<const char*>(&defaulted), sizeof(defaulted));
        }

        template<>
        inline void write<void>(OStream& stream)
        {}

    } // END: utility
    
#if SUB0PUB_STD
    typedef std::ostream OStream;
    typedef std::istream IStream;
#else
    typedef utility::OStream OStream;
    typedef utility::IStream IStream;
#endif

    /** Broker manages publisher-subscriber connection for a 'Data' type
     * @tparam Data  Data type which this instance manages connections for
     */
    template< typename Data >
    class Broker;

    /** Base type for publishing signals of 'Data' type
     * @tparam Data  Data type which this instance manages publishing for
     */
    template< typename Data >
    class Publish;

    /** Base type for subscription to receive signals of 'Data' type
     * @tparam Data  Data type which this instance manages subscriptions for
     */
    template< typename Data >
    class Subscribe;
    
    /** Internal configured details for tracing and error handling
     */
    namespace detail
    {
        /** Provides debug assertion/exception checks for Broker<>
         * @tparam cMessageTrace   Enable logging for broker events
         * @tparam cDoAssert         Enable assertion tests for invalid parameters
         */
        template< const bool cMessageTrace = false, const bool cDoAssert = true >
        struct CheckT
        {
            /** Diagnose creation of new subscriber
             * @param broker  Broker instance that manages the connection
             * @param subscriber  Subscriber to be registered into the broker
             * @param subscriptionCount  Count of existing registered subscriptions on the broker
             * @param subscriptionCapacity  Count specifying subscriptionCount limit for the broker
             */
            template<typename Data>
            inline static void onSubscription( const Broker<Data>& broker, Subscribe<Data>* subscriber, const uint32_t subscriptionCount, const uint32_t subscriptionCapacity )
            {
                if ( cDoAssert )
                {
                    assert( subscriber );
                    assert( subscriptionCount < subscriptionCapacity );
                }
#if SUB0PUB_TRACE /// @todo iostream removal:
                if ( cMessageTrace )
                { 
                    std::cout << "[Sub0Pub] New Subscription " << *subscriber << " for Broker<" << broker.typeName() << ">{" << broker << '}' << std::endl;
                }
#endif
            }

            /** Diagnose creation of new publisher
             * @param publisher  Publisher to be registered into the broker
             * @param broker  Broker instance that manages the connection
             * @param publisherCount  Count of existing registered publishers on the broker
             * @param publisherCapacity  Count specifying publisherCount limit for the broker
             */
            template<typename Data>
            inline static void onPublication( Publish<Data>* publisher, const Broker<Data>& broker, const uint32_t publisherCount, const uint32_t publisherCapacity )
            {
                if ( cDoAssert )
                {
                    assert( publisher );
                    assert( publisherCount < publisherCapacity );
                }
#if SUB0PUB_TRACE /// @todo iostream removal:
                if ( cMessageTrace )
                { 
                    std::cout << "[Sub0Pub] New Publication " << *publisher << " for Broker<" << broker.typeName() << ">{" << broker << '}' << std::endl;
                }
#endif
            }

            /** Diagnose data publish event
             * @param publisher  Publisher that is sending the data
             * @param data  The data to be published
             */
            template<typename Data>
            inline static void onPublish( const Publish<Data>& publisher, const Data& data )
            {
#if SUB0PUB_TRACE /// @todo iostream removal: 
                if ( cMessageTrace )
                {
                    (void)data; ///< @todo Data serialize
                    std::cout << "[Sub0Pub] Published " << publisher
                        << " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << Broker<Data>::typeName() << ']' << std::endl;
                }
#endif
            }

            /** Diagnose data receive event
             * @param subscriber  Subscriber that is receiving the data
             * @param data  The data that is received
             */
            template<typename Data>
            static void 
#if __GNUG__ /// @todo GCC 7.2(TBC) publish(const Data & data) bug on inlining this function and calling detail::Check::onReceive()?
                __attribute__((noinline)) 
#endif
                onReceive( Subscribe<Data>* subscriber, const Data& data )
            {
                if ( cDoAssert )
                {
#if 0 ///< @temp
                    while (!subscriber)
                    {
                        delay(1);
                    }
#endif
                    if ( subscriber == nullptr )
                    {
                       // while (true) {};
                       assert(subscriber );
                    }
                }
#if SUB0PUB_TRACE /// @todo iostream removal: 
                if ( cMessageTrace )
                {
                    (void)data; ///< @todo Data serialize
                    std::cout << "[Sub0Pub] Received " << *subscriber
                        << " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << Broker<Data>::typeName() << ']' << std::endl;
                }
#endif
            }
        };

        /** Runtime checker type with support for assert/exception/trace etc
         */
        typedef CheckT<SUB0PUB_TRACE,SUB0PUB_ASSERT> Check;
    } // END: detail

#pragma warning(push)
#pragma warning(disable:4355) ///< warning C4355: 'this' : used in base member initializer list

    /** Base type for an object that subscribes to some strong-typed Data
     * @tparam  Data  Type that will be received from publishers of corresponding type
     */
    template< typename Data >
    class Subscribe
    {
    public:
        /** Registers the subscriber within the broker framework
         * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names 'may' be used.
         */
        Subscribe( 
#if SUB0PUB_TYPEIDNAME
            const uint32_t typeId = 0, const char* typeName = 0/*nullptr*/ 
#endif
        )
        : broker_( this
#if SUB0PUB_TYPEIDNAME
            , typeId, typeName 
#endif
        )
        {}

        virtual ~Subscribe()
        {  broker_.unsubscribe(this); } ///< @todo Make implicit broker handle
        
        /** Receive published Data
         * @remark Data is published from Publish<Data>::publish
         */
        virtual void receive( const Data& data ) = 0;

        virtual bool filter(const Data& data)
        {  return true; }

#if SUB0PUB_TYPEIDNAME
        /** Get name identifier of the Data from the broker
         * @return Broker null-terminated type name
        */
        const char* typeName() const
        { return broker_.typeName(); }

        /** Stream operator for diagnostics reporting
         * @param stream  Stream to report into
         * @param subscriber  Subscriber instance to be written into stream
         * @return Reference to 'stream'
         */
        friend OStream& operator<< ( OStream& stream, const Subscribe<Data>& subscriber )
        { return stream << subscriber.typeName() << '{' << (void*)&subscriber << '}'; }
#endif

    private:
        Broker<Data> broker_; ///< MonoState broker instance to manage publish-subscribe connections
    };

    /** Base type for an object that publishes to some strong-typed Data
     * @tparam  Data  Type that will be published by this object to subscribers of corresponding type
     */
    template< typename Data >
    class Publish
    {
    public:
        /** Registers the publisher within the broker framework
         * @param[in] typeName Optional unique data name given to data for inter-process signaling. @warning If not supplied non-portable compiler generated names 'may' be used.
         */
        Publish(
#if SUB0PUB_TYPEIDNAME
            const uint32_t typeId = 0, const char* typeName = 0/*nullptr*/
#endif
        )
        : broker_( this
#if SUB0PUB_TYPEIDNAME
            , typeId, typeName
#endif
        )
        {}

        virtual ~Publish()
        { broker_.unsubscribe(this); } ///< @todo Make implicit broker handle

        /** Publish data to subscribers
         * @param[in]  data  Data value to publish to subscribers
         * @remark Data will be received by Subscribe<Data>::receive
         */
        void publish( const Data& data ) const
        {
            detail::Check::onPublish( *this, data );
            broker_.publish(data);
        }

#if SUB0PUB_TYPEIDNAME
        /** Get name identifier of the Data from the broker
         * @return Broker null-terminated type name
        */
        const char* typeName() const
        { return broker_.typeName(); }

        /** Get unique identifier of the Data from the broker
         * @return Broker unique type index
        */
        uint32_t typeId() const
        { return broker_.typeId(); }

        /** Stream operator for diagnostics reporting
         * @param stream  Stream to report into
         * @param publisher  Publisher instance to be written into stream
         * @return Reference to 'stream'
         */
        friend OStream& operator<< ( OStream& stream, const Publish<Data>& publisher )
        { return stream << publisher.typeName() << '{' << (void*)&publisher << '}'; }
#endif

    private:
        Broker<Data> broker_; ///< MonoState broker instance to manage publish-subscribe connections
    };

#pragma warning(pop)

    /** Broker manages publisher-subscriber connection for a data-type
     * @tparam Data  Data type which this instance manages connections for
     * @todo Cross-module support
     */
    template< typename Data >
    class Broker
    {
    public:
        static const uint32_t cMaxSubscriptions = 8U; ///< Subscription limit in fixed table per broker

    public:
        /** Registers subscriber in brokers subscription table
         * @param[in] typeName Optional unique data name given to data for inter-process signaling. 
         * @warning If typeName not supplied compiler generated names 'may' be used which are non-portable between vendors.
         */
        Broker( Subscribe<Data>* subscriber
#if SUB0PUB_TYPEIDNAME
            , const uint32_t typeId = 0, const char* typeName = 0/*nullptr*/ 
#endif
        )
        {
            detail::Check::onSubscription( *this, subscriber, state_.subscriptionCount, cMaxSubscriptions );
#if SUB0PUB_TYPEIDNAME
            setDataName(typeId, typeName);
#endif
            state_.subscriptions[state_.subscriptionCount++] = subscriber;
        }

        /** Validated publication
         * @remark No record of publishers of data is currently maintained
         * @param[in] typeName Optional unique data name given to data for inter-process signalling. 
         * @warning If typeName not supplied compiler generated names 'may' be used which are non-portable between vendors.
         */
        Broker ( Publish<Data>* publisher
#if SUB0PUB_TYPEIDNAME
            , const uint32_t typeId = 0, const char* typeName = 0/*nullptr*/
#endif
        )
        {
            detail::Check::onPublication( publisher, *this, 0, 1/* @note No limit at present */ );
#if SUB0PUB_TYPEIDNAME
            setDataName(typeId, typeName);
#endif
            // Do nothing for now...
        }

        void unsubscribe(Subscribe<Data>* subscriber)
        {
            Subscribe<Data>** const iBegin = state_.subscriptions;
            Subscribe<Data>** const iEnd = iBegin + state_.subscriptionCount;
            Subscribe<Data>** const iPend = std::remove(iBegin, iEnd, subscriber );
            assert(std::distance(iEnd, iPend) == -1);
            --state_.subscriptionCount;
        }

        void unsubscribe(Publish<Data>* publisher)
        {
            // Do nothing for now...
        }

#if SUB0PUB_TYPEIDNAME
        /** Set a unique identifier for the data the broker manages
         * @remark This name is used during serialisation for inter-process communications
         * @param[in]  typeName  Null terminated compile-time string constant
         */
        void setDataName(const uint32_t typeId, const char* const typeName )
        {
            if (typeId)
            {
                // Check if assigning a different name or Id is when already set
                assert( !state_.typeId || (state_.typeId==typeId) );// @todo use RuntimeCheck and handle if a subscriber uses a different name better
                state_.typeId = typeId; /// @todo sub0::utility::hash(state_.typeName); // Cache hash result @todo Make compile time
            }

            if (typeName)
            {
                // Check if assigning a different name or Id is when already set
                assert( !state_.typeName || (std::strcmp(state_.typeName,typeName)==0) );// @todo use RuntimeCheck and handle if a subscriber uses a different name better
                state_.typeName = typeName;
            }
        }
#endif

        /** Send data to registered subscribers
         * @param data  Data sent to subscribers via their 'receive()' function
         */
        void publish(const Data & data) const
        {
            for (uint32_t iSubscription = 0U; iSubscription < state_.subscriptionCount; ++iSubscription )
            {
                Subscribe<Data>* subscription = state_.subscriptions[iSubscription];
                detail::Check::onReceive( subscription, data );

                if ( subscription->filter(data))
                {
                    subscription->receive(data);
                }
            }
        }

        /** Prints address of monotonic state
         * @param stream  Stream to output into
         * @param broker  Broker instance to output for
         * @return The 'stream' instance
         */
#if 0 ///@todo Remove unecessary stream operations: 
        friend OStream& operator<< ( OStream& stream, const Broker<Data>& broker )
        {
            return stream << (void*)&broker.state_;
        }
#endif

#if SUB0PUB_TYPEIDNAME
        /** @return Unique identifier index for inter-process binary connections
         */
        static uint32_t typeId()
        {
            return state_.typeId;
        }

        /** @return Unique identifier name for inter-process text connections
         */
        static const char* typeName()
        {
            return state_.typeName;
        }
#endif

    private:
        /** Object state as monotonic object shared by all instances
         */
        struct State
        {
            uint32_t subscriptionCount; ///< Count of subscriptions_
            Subscribe<Data>* subscriptions[cMaxSubscriptions];    ///< Subscription table @todo More flexible count-support
#if SUB0PUB_TYPEIDNAME
            uint32_t typeId; ///< Type identifier index or name hash
            const char* typeName; ///< user defined data name overrides non-portable compiler-generated name
#endif
            State() 
                : subscriptionCount(0)
                , subscriptions() 
            {}
        };
        static State state_; ///< MonoState subscription table
    };

    /** Monotonic broker state
     * @todo State should be shared across module boundaries and owned/defined in a single module e.g. std::cout like singleton
     */
    template<typename Data>
    typename Broker<Data>::State Broker<Data>::state_ = Broker<Data>::State();

    /** Explicit allocation of monotonic state
    @note Enables appearing within Globals for ELF embedded targets
    */
#define SUB0_BROKERSTATE(Data) \
    namespace sub0 {  template<> Broker<Data>::State Broker<Data>::state_ = Broker<Data>::State(); } 
    
    /** Publish data, used when inheriting from multiple Publish<> base types
     * @remark Circumvents C++ Name-Hiding limitations when multiple Publish<> base types are present 
        i.e. publish( 1.0F) is ambiguous in this case.
     * @note Compiler error will occur if From does not inherit Publish<Data>
     *
     * @param[in] from  Producer object inheriting from one or more Publish<> objects
     * @param[in] data  Data that will be published using the base Publish<Data> object of From
     */
    template<typename From, typename Data>
    inline void publish(const From& from, const Data& data)
    {
        const Publish<Data>& publisher = from;
        publisher.publish(data);
    }

    
    /** Interface for data provider to indicate destination buffer status
     * @see ForwardPublish
     */
    class IBufferPublish
    {
    public:

        /** Notify that the buffered data is fully populated
         */
        virtual void onBufferComplete() = 0;
    };

    template< typename Prefix_t, typename Header_t, typename Postfix_t >
    class BinaryWriter
    {
    public:
        /** Output header and pay-load for data as binary
         * @param stream  Stream to write into
         * @param data  Data to construct a header record and data payload for
         */
        template<typename Data>
        inline void write(OStream& stream, const Data& data) const
        {
            utility::write<Prefix_t>(stream);
            Header_t header(data);
            stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
            stream.write(reinterpret_cast<const char*>(&data), sizeof(data));
            utility::write<Postfix_t>(stream);
        }

    };

    template< typename Prefix_t, typename Header_t, typename Postfix_t >
    class BinaryReader
    {
        enum class State { Prefix, Header, Data, Postfix/*, SyncLost*/ , COUNT_ };
    public:
        BinaryReader()
            : currentBuffer_()
            , state_()
#if 0 /// @todo Handle void Prefix_t
            , prefix_();
#endif
            , header_()
            , postfix_()
            , bufferRegistry_()
            , bufferRegistryEnd_(bufferRegistry_.begin())
        {}

        /** Register a sink to the specified typed Data buffer
         * @remark Performs insertion sorting on buffers by the IBufferPublish::typeId() for the buffer
         * @remark Called by sub0::ForwardPublish<Data>
         *
         * @param bufferPublisher  Buffer handling object to store and signal data completion
         */
        template < typename Data >
        void setBufferPublisher(Data& buffer, IBufferPublish* bufferPublisher)
        {
            assert(!currentBuffer_.buffer); // We don't intend to support adding buffers at runtime
            setBufferPublisher(   { Header_t(buffer)
                                , { reinterpret_cast<char*>(&buffer), static_cast<uint_fast16_t>(sizeof(buffer)), bufferPublisher } } );
        }

        bool read(IStream& stream)
        {
            /// Read data until an incomplete message
            while (readBuffer(stream))
            {
                if (state_ == State::Header)
                    return true; ///< @return True = Completed reading a payload so allow processing by caller
            }

            return false; ///< @return False = Need more data
        }

    private:

        struct BufferPublisher
        {
            char* buffer; ///< Data buffer
            uint_fast16_t bufferSize; ///< size of buffer
            IBufferPublish* bufferPublisher; ///< Type specific publish of buffer
        };


        /** Returns/finds buffer for state
        */
        BufferPublisher findStateBuffer(const State state)
        {
            switch (state)
            {
            default: //< @todo unreachable
            case State::Prefix:  return { nullptr , 0U , nullptr };///< @todo Handle non-void Prefix_t: { &prefix_ , sizeof(prefix_) , nullptr }
            case State::Header:  return { (char*)&header_ , static_cast<uint_fast16_t>(sizeof(header_)) , nullptr };
            case State::Data:    return findBufferPublisher(header_);
            case State::Postfix: return { (char*)&postfix_ , static_cast<uint_fast16_t>(sizeof(postfix_)), currentBuffer_.bufferPublisher };///< @todo Handle void Postfix_t: { nullptr , 0U , nullptr }
            }
        }
        
        /** Read payload data form stream and detect payload completion
         * @return True when data packet(s) have been published, false if no completed packet was present in stream
        */
        bool readBuffer(IStream& stream)
        {
#if SUB0PUB_STD
            const uint_fast16_t readCount = static_cast<uint_fast16_t>(stream.read(currentBuffer_.buffer, currentBuffer_.bufferSize).gcount()); ///< @todo readsome() for async
#else
            const uint_fast16_t readCount = stream.read(currentBuffer_.buffer, currentBuffer_.bufferSize);
#endif
            currentBuffer_.buffer += readCount;
            currentBuffer_.bufferSize -= readCount;
            return (currentBuffer_.bufferSize==0) ? stateComplete() : false;
        }

        bool checkStateOk(const State state) const
        {
            switch (state)
            {
            default: //< @todo unreachable
            case State::Prefix:  return true; ///< @todo Handle non-void Prefix_t: (prefix_ == Postfix_t())
            case State::Header:  return true; ///< @todo Check on header?
            case State::Data:    return true;
            case State::Postfix: return postfix_ == Postfix_t();///< @todo Handle void Postfix_t
            }
        }

        inline State nextState(const State currentState ) const
        {
            switch (currentState)
            {
            default: //< @todo unreachable
            case State::Prefix:  return State::Header;
            case State::Header:  return State::Data;
            case State::Data:    return !std::is_same<Postfix_t,void>::value ? State::Postfix : nextState(State::Postfix); ///< @note may not have Prefix_t or Postfix_t
            case State::Postfix:
                assert(currentBuffer_.bufferPublisher);
                assert(checkStateOk(currentState) ); ///< @todo Handle missing/corrupted postfix
                if (currentBuffer_.bufferPublisher)
                    currentBuffer_.bufferPublisher->onBufferComplete(); // Signal completion of buffer content to publish data signal
                return !std::is_same<Prefix_t,void>::value ? State::Prefix : nextState(State::Prefix);
            }
        }

        bool stateComplete()
        {
            state_ = nextState( state_ );
            currentBuffer_ = findStateBuffer(state_);

            // Check if header maps to a recognised Data
            if ( currentBuffer_.buffer == nullptr)/// @todo Does not handle and discard unrecognised typeId [Critical]
            {
                if ( state_ == State::Data )
                    assert((void*)0 == "Data of 'expectedTypeName' is not size 'expectedPayloadSize'"); /// @todo Does not handle changed data structure size [Critical]
                else
                    assert((void*)0 == "Agh - some logic is wrong!");
            }
            
            return currentBuffer_.buffer != nullptr;
        }


        /** Defines the maximum number of Data type buffers the deserialiser can store
         * @todo Make this figure compile-time configurable instead of arbitrary size!
         */
        static const uint32_t cMaxDataBufferCount = 64U;
        typedef std::pair< Header_t, BufferPublisher> HeaderToBufferPublisher;

        /** @see setBufferPublisher(Data&, IBufferPublish*) */
        void setBufferPublisher(const HeaderToBufferPublisher& headerToBufferPublisher )
        {
            /// @todo make this a linked list to remove capacity limitations?
            typename BufferForDataArray::iterator insertionPoint = std::upper_bound( std::begin(bufferRegistry_), bufferRegistryEnd_, headerToBufferPublisher, 
                [](const HeaderToBufferPublisher& lhs, const HeaderToBufferPublisher& rhs) { return lhs.first < rhs.first; } );
            *bufferRegistryEnd_ = headerToBufferPublisher; //< Insert at end
            std::rotate(insertionPoint, bufferRegistryEnd_, bufferRegistryEnd_ + 1U); // Rotate the entries after the insertion point to move the new value into the insertion point
            ++bufferRegistryEnd_;
        }

        BufferPublisher findBufferPublisher(const Header_t header)
        {
            typename BufferForDataArray::iterator iFind = std::lower_bound(std::begin(bufferRegistry_), bufferRegistryEnd_, HeaderToBufferPublisher(header, BufferPublisher())
               , [](const HeaderToBufferPublisher& lhs, const HeaderToBufferPublisher& rhs) { return lhs.first < rhs.first; } );
            
            if ((iFind != bufferRegistryEnd_) && (iFind->first == header))
                return iFind->second;
            else
                return { nullptr , 0U , nullptr };
        }

        typedef std::array<HeaderToBufferPublisher, cMaxDataBufferCount> BufferForDataArray;

        BufferPublisher currentBuffer_; ///< Current prefix/header/payload/postfix buffer
        State state_; ///< Which buffer is being read

#if 0 /// @todo Handle void Prefix_t
        Prefix_t prefix_;
#endif
        Header_t header_; ///< Packet head buffer
        Postfix_t postfix_;


        BufferForDataArray bufferRegistry_;
        typename BufferForDataArray::iterator bufferRegistryEnd_; ///< Iterator to end of bufferRegistry_ @note Count = bufferRegistryEnd_-bufferRegistry_
    };

    /** Binary protocol for serialised signal and data transfer
     * @remark The protocol consists of a Header chunk followed by Header::dataBytes bytes of payload data
     */
    class DefaultSerialisation
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
                : typeId(Broker<Data>::typeId() )
#else

                : typeId(12345) // reinterpret_cast<uint32_t>(&typeid(data)) ) ///< @todo Crude using ypeid address!!!
#endif
                , dataBytes(sizeof(Data) )
            {}

            /** Sort by typeId only
            */
            bool operator < (const Header& rhs) { return typeId < rhs.typeId; }

            /** Compare full equality 
            */
            bool operator == (const Header& rhs) { return (typeId == rhs.typeId) && (dataBytes == rhs.dataBytes); }
        };

        struct Postfix
        {
            const uint8_t delim = '\n';
        };

        using Writer = BinaryWriter<Prefix, Header, Postfix>;
        using Reader = BinaryReader<Prefix, Header, Postfix>;
    };

    /** Serialises Sub0Pub data into a target stream object
     * @remark Serialised data can be received and published using the counterpart StreamDeserializer instance
     * @remark Can be used to create inter-process transfers very easily using the specified Protocol @see sub0::DefaultSerialisation
     * @tparam  Protocol  Stream data protocol to use defining how the data header and payload is structured
     */
    template< typename Protocol = DefaultSerialisation >
    class StreamSerializer
    {
    public:
        /** Construct from stream
         * @param[in] stream  Stream reference stored and used to write serialised data into
         */
        StreamSerializer( OStream& stream )
        : stream_(stream)
        , writer_()
        {}

        /** Receives forwarded data from a subscriber and serialises it to the output stream
         * @param[in] data  Forwarded data
         */
        template<typename Data>
        void forward( const Data& data )
        {
            writer_.write( stream_, data );
        }

    private:
        OStream& stream_; ///< Stream into which data is serialised
        typename Protocol::Writer writer_;
    };


    /** Publishes messages from a serialised-input stream using the specified Protocol 
     * @remark StreamDeserializer can be used for inter-process or distributed systems over a network where the stream
     *  could be a TcpStream or could be a file in simple cases. The serialised data is expected to be generated from a
     *  corresponding StreamSerializer instance for the same Protocol.
     * @tparam  Protocol  Stream data protocol to use defining how the data header and payload is structured
     */
    template< typename Protocol = DefaultSerialisation >
    class StreamDeserializer
    {
    public:
        /** Store reference to supplied IStream which will be read on update()
        */
        StreamDeserializer( IStream& stream )
            : stream_(stream)
            , reader_()
        {}

        template < typename Data >
        void setBufferPublisher( Data& buffer, IBufferPublish* bufferPublisher )
        {
            reader_.setBufferPublisher( buffer, bufferPublisher );
        }

        /** Polls data from the input stream
         * @return True when data packet(s) have been published, false if no completed packet was present in stream
         */
        bool update()
        {
            return reader_.read(stream_);
        }

    private:
        IStream& stream_; ///< Stream from which data is deserialised
        typename Protocol::Reader reader_;
        uint8_t* buffer_;
        uint32_t bufferSize_;
        uint32_t currentBufferReadCount_; ///< NUmber of bytes read for the header or payload depending on currentBuffer_!=nullptr
    };


    /** Forward receive() to  Target type convertible from this
     * @remark The call is made with Data type allowing for templated forward() handler functions @see class StreamSerializer
     * @note This uses the CRTP(curiously recurring template pattern) to forward to a target type derived from ForwardSubscribe<..>
     * @tparam  Data  Data type which will be forwarded to the derived Target implementation
     * @tparam  Target  Type of derived class which implements a function of type Target::forward( const Data& data ) via base inheritance or direct member
     */
    template<typename Data, typename Target >
    class ForwardSubscribe : public Subscribe<Data>
    {
    public:
        /** Create subscription to the data type
         * @param typeName  Unique name given to the serialised data entry @note Replaces compiler generated name which is not portable
         */
        ForwardSubscribe(
#if SUB0PUB_TYPEIDNAME
            const uint32_t typeId = 0, const char* typeName = 0/*nullptr*/
#endif
        )
            : Subscribe<Data>(
#if SUB0PUB_TYPEIDNAME
                typeId, typeName
#endif
                )
        {}

        /** Receives subscribed data and forward to target object
         * @param data  Data to forward
         */
        inline virtual void receive( const Data& data )
        {
            static_cast<Target*>(this)->forward( data );
        }
    };

    /** Register publication of data with a provider instance
     * @remark The call is made with Data type allowing for templated forward() handler functions @see class StreamSerializer
     * @note This uses the CRTP(curiously recurring template pattern) to forward to a target type derived from ForwardPublish<..>
     * @tparam  Data  Data type which will be read into from a Provider
     * @tparam  Provider  Type of derived class which implements a function of type Provider::addSink( Data* bufferPublisher ) via base inheritance or direct member
     *
     * @todo API not final
     */
    template<typename Data, typename Provider >
    class ForwardPublish : public Publish<Data>, protected IBufferPublish
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
            , IBufferPublish()
        {
            static_cast<Provider*>(this)->setBufferPublisher( buffer_, static_cast<IBufferPublish*>(this) ); // Register the buffer sink to the data provider
        }

    private:

        /** Called by Provider to notify when the buffer_ has been populated
         */
        virtual void onBufferComplete()
        { Publish<Data>::publish( buffer_ ); }

    private:
        Data buffer_; ///< Data buffer instance 
                      ///< @todo Double-buffer data storage for asynchronous processing and receive?
    };

} // END: sub0

#endif
