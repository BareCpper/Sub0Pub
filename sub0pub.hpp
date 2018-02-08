/** Sub0Pub - Near-Compile time data Publisher & Subscriber model for embedded, desktop, and distributed systems
    Copyright (C) 2018 Craig Hutchinson (craig-sub0pub@crog.uk)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or any
    later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>


    Original source is available at https://github.com/Crog/Sub0Pub/blob/master/sub0pub.hpp
*/
#ifndef CROG_SUB0PUB_HPP
#define CROG_SUB0PUB_HPP

#include <algorithm>
#include <cassert> // assert
#include <cstdint> // uint32_t
#include <iostream> // std::cout

/** Define SUB0PUB_TRACE=true to enable message logging to std::cout for event trace, SUB0PUB_TRACE=false
*/
#ifndef SUB0PUB_TRACE
#define SUB0PUB_TRACE false ///< Disable Trace logging to std::cout by default
#endif

/** Define SUB0PUB_ASSERT=true to enable assertion checks for events, SUB0PUB_ASSERT=false to disable
*/
#ifndef SUB0PUB_ASSERT
#define SUB0PUB_ASSERT true ///< Enable assertion tests by default
#endif

namespace sub0
{
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
    }

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
            static void onSubscription( const Broker<Data>& broker, SubscribeTo<Data>* subscriber, const uint32_t subscriptionCount, const uint32_t subscriptionCapacity )
            {
                if ( cDoAssert )
                {
                    assert( subscriber != nullptr );
                    assert( subscriptionCount < subscriptionCapacity );
                }
                if ( cMessageTrace )
                {
                    std::cout << "[Sub0Pub] New Subscription " << *subscriber << " for Broker<" <<  typeid(Data).name () << ">{" << broker << '}' << std::endl;
                }
            }

            /** Diagnose creation of new publisher
             * @param publisher  Publisher to be registered into the broker
             * @param broker  Broker instance that manages the connection
             * @param publisherCount  Count of existing registered publishers on the broker
             * @param publisherCapacity  Count specifying publisherCount limit for the broker
             */
            template<typename Data>
            static void onPublication( PublishTo<Data>* publisher, const Broker<Data>& broker, const uint32_t publisherCount, const uint32_t publisherCapacity )
            {
                if ( cDoAssert )
                {
                    assert( publisher != nullptr );
                    assert( publisherCount < publisherCapacity );
                }
                if ( cMessageTrace )
                {
                    std::cout << "[Sub0Pub] New Publication " << *publisher << " for Broker<" <<  typeid(Data).name () << ">{" << broker << '}' << std::endl;
                }
            }

            /** Diagnose data publish event
             * @param publisher  Publisher that is sending the data
             * @param data  The data to be published
             */
            template<typename Data>
            static void onPublish( const PublishTo<Data>& publisher, const Data& data )
            {
                if ( cMessageTrace )
                {
                    std::cout << "[Sub0Pub] Published " << publisher
                        << " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << typeid(Data).name () << ']' << std::endl;
                }
            }

            /** Diagnose data receive event
             * @param subscriber  Subscriber that is receiving the data
             * @param data  The data that is received
             */
            template<typename Data>
            static void onReceive( SubscribeTo<Data>* subscriber, const Data& data )
            {
                if ( cDoAssert )
                {
                    assert( subscriber != nullptr );
                }
                if ( cMessageTrace )
                {
                    std::cout << "[Sub0Pub] Received " << *subscriber
                        << " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << typeid(Data).name () << ']' << std::endl;
                }
            }
        };

        /** Runtime checker type wiht suport for assert/exception/trace etc
         */
        typedef CheckT<SUB0PUB_TRACE,SUB0PUB_ASSERT> Check;

    }

    /** Broker manages publisher-subscriber connection for a 'Data' type
     * @tparam Data  Data type which this instance manages connections for
     */
    template< typename Data >
    class Broker;

    /** Base type for publishing signals of 'Data' type
     * @tparam Data  Data type which this instance manages publishing for
     */
    template< typename Data >
    class PublishTo;

    /** Base type for subscription to receive signals of 'Data' type
     * @tparam Data  Data type which this instance manages subscriptions for
     */
    template< typename Data >
    class SubscribeTo;


#pragma warning(push)
#pragma warning(disable:4355) ///< warning C4355: 'this' : used in base member initializer list

    /** Base type for an object that subscribes to some strong-typed Data
     * @tparam  Data  Type that will be received from publishers of corresponding type
     */
    template< typename Data >
    class SubscribeTo
    {
    public:
        /** Registers the subscriber within the broker framework
         * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names will be used.
         */
        SubscribeTo( const char* typeName = nullptr )
        : broker_( this, typeName )
        {}

        /** Empty
         */
        virtual ~SubscribeTo()
        {}

        /** Receive published Data
         * @remark Data is published from PublishTo<Data>::publish
         */
        virtual void receive( const Data & data ) = 0;

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
        friend std::ostream& operator<< ( std::ostream& stream, const SubscribeTo<Data>& subscriber )
        { return stream << subscriber.typeName() << '{' << (void*)&subscriber << '}'; }

    private:
        Broker<Data> broker_; ///< MonoState broker instance to manage publish-subscribe connections
    };

    /** Base type for an object that publishes to some strong-typed Data
     * @tparam  Data  Type that will be published by this object to subscribers of corresponding type
     */
    template< typename Data >
    class PublishTo
    {
    public:
        /** Registers the publisher within the broker framework
         * @param[in] typeName Optional unique data name given to data for inter-process signaling. @warning If not supplied non-portable compiler generated names will be used.
         */
        PublishTo( const char* typeName = nullptr )
        : broker_( this, typeName )
        {}

        /** Publish data to subscribers
         * @param[in]  data  Data value to publish to subscribers
         * @remark Data will be received by SubscribeTo<Data>::receive
         */
        void publish( const Data& data ) const
        {
            detail::Check::onPublish( *this, data );
            broker_.publish (data);
        }

        /** Get name identifier of the Data from the broker
         * @return Broker null-terminated type name
         */
        const char* typeName() const
        { return broker_.typeName(); }

        /** Stream operator for diagnostics reporting
         * @param stream  Stream to report into
         * @param publisher  Publisher instance to be written into stream
         * @return Reference to 'stream'
         */
        friend std::ostream& operator<< ( std::ostream& stream, const PublishTo<Data>& publisher )
        { return stream << publisher.typeName() << '{' << (void*)&publisher << '}'; }

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
        static const uint32_t cMaxSubscriptions = 5U; ///< Subscription limit in fixed table per broker

    public:
        /** Registers subscriber in brokers subscription table
         * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names will be used.
         */
        Broker ( SubscribeTo<Data>* subscriber, const char* typeName = nullptr )
        {
            detail::Check::onSubscription( *this, subscriber, state_.subscriptionCount, cMaxSubscriptions );
            setDataName(typeName);
            state_.subscriptions[state_.subscriptionCount++] = subscriber;
        }

        /** Validated publication
         * @remark No record of publishers of data is currently maintained
         * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names will be used.
         */
        Broker ( PublishTo<Data>* publisher, const char* typeName = nullptr )
        {
            detail::Check::onPublication( publisher, *this, 0, 1/* @note No limit at present */ );
            setDataName(typeName);
            // Do nothing for now...
        }

        /** Set a unique identifier for the data the broker manages
         * @remark This name is used during serialisation for inter-process communications
         * @param[in]  typeName  Null terminated compile-time string constant
         */
        void setDataName( const char* const typeName )
        {
            if ( typeName == nullptr )
                return;

            // @todo use RuntimeCheck and handle if a subscriber uses a different name better
            assert( (state_.typeName==nullptr) || (strcmp(state_.typeName,typeName)==0) );
            state_.typeName = typeName;
            state_.typeId = sub0::utility::hash( state_.typeName ); // Cache hash result @todo Make compile time

        }

        /** Send data to registered subscribers
         * @param data  Data sent to subscribers via their 'receive()' function
         */
        void publish (const Data & data) const
        {
            const uint32_t subscriptionCount = std::min( state_.subscriptionCount, cMaxSubscriptions );
            for ( uint32_t iSubscription = 0U; iSubscription < subscriptionCount; ++iSubscription )
            {
                SubscribeTo<Data>* subscription = state_.subscriptions[iSubscription];
                detail::Check::onReceive( subscription, data );
                subscription->receive(data);
            }
        }

        /** Prints address of monotonic state
         * @param stream  Stream to output into
         * @param broker  Broker instance to output for
         * @return The 'stream' instance
         */
        friend std::ostream& operator<< ( std::ostream& stream, const Broker<Data>& broker )
        { return stream << (void*)&broker.state_; }

        /** @return Unique identifier index for inter-process binary connections
        */
        static uint32_t typeId()
        { return state_.typeId; }

        /** @return Unique identifier name for inter-process text connections
        */
        static const char* typeName()
        { return state_.typeName != nullptr ? state_.typeName : typeid(Data).name(); }

    private:
        /** Object state as monotonic object shared by all instances
        */
        struct State
        {
            uint32_t subscriptionCount; ///< Count of subscriptions_
            SubscribeTo<Data>* subscriptions[cMaxSubscriptions];    ///< Subscription table @todo More flexible count-support
            uint32_t typeId; ///< Type identifier index or name hash
            const char* typeName; ///< user defined data name overrides non-portable compiler-generated name
        };
        static State state_; ///< MonoState subscription table
    };

    /** Monotonic broker state
     * @todo State should be shared across module boundaries and owned/defined in a single module e.g. std::cout like singleton
     */
    template<typename Data>
    typename Broker<Data>::State Broker<Data>::state_ = Broker<Data>::State();

    /** Publish data, used when inheriting from multiple PublishTo<> base types
     * @remark Circumvents C++ Name-Hiding limitations when multiple PublishTo<> base types are present i.e. publish( 1.0F) is ambiguous in this case.
     * @note Compiler error will occur if From does not inherit PublishTo<Data>
     *
     * @param[in] from  Producer object inheriting from one or more PublishTo<> objects
     * @param[in] data  Data that will be published using the base PublishTo<Data> object of From
     */
    template<typename From, typename Data>
    void publish( const From* from, const Data& data )
    {
        const PublishTo<Data>& publisher = *from;
        publisher.publish( data );
    }

    /** Binary protocol for serialised signal and data transfer
     * @remark The protocol consists of a Header chunk followed by Header::dataBytes bytes of payload data
     */
    struct BinaryProtocol
    {
        /** Header containing signal type information
        */
        struct Header
        {
            static const uint32_t cMagic = sub0::utility::FourCC<'S','U','B','0'>::value; //< Magic number to identify Sub0 network protocol packets

            uint32_t magic; ///< FourCC identifier containing 'SUB0'
            uint32_t typeId; ///< Data type identifier @note The Id may be user specified for inter-process
            uint32_t dataBytes; ///< Count of bytes that follow after the header data
        };

        /** Output header for data as binary
         * @param stream  Stream to write into
         * @param data  Data to construct a header record for
         */
        template<typename Data>
        static void writeHeader( std::ostream& stream, const Data& data )
        {
            Header header = { Header::cMagic
                             , Broker<Data>::typeId()
                             , sizeof(data) };
            stream.write( reinterpret_cast<const char*>(&header), sizeof(header) );
        }

        /** Output data pay-load as binary
         * @param stream  Stream to write into
         * @param data  Data to write data from
         */
        template<typename Data>
        static void writePayload( std::ostream& stream, const Data& data )
        { stream.write( reinterpret_cast<const char*>(&data), sizeof(data) ); }
    };

    /** Serialises Sub0Pub data into a target stream object
    * @remark Implements a basic inter process transfer protocol @see sub0::BinaryProtocol
    */
    template< typename Protocol = BinaryProtocol >
    class StreamSerializer
    {
    public:
        /** Construct from stream
         * @param[in] stream  Stream reference stored and used to write serialised data into
         */
        StreamSerializer( std::ostream& stream )
        : stream_(stream)
        {}

        /** Empty
         */
        virtual ~StreamSerializer()
        {}

        /** Receives forwarded data from a subscriber and serialises it to the output stream
         * @param[in] data  Forwarded data
         */
        template<typename Data>
        void forward( const Data& data )
        {
            Protocol::writeHeader( stream_, data );
            Protocol::writePayload( stream_, data );
            stream_ << '\n';
        }

    private:
        std::ostream& stream_;
    };

    /** Forward receive() to  Target type convertible from this
    * @remark The call is made with Data type allowing for templated forward() handler functions @see class Stream
    * @note This uses the CRTP(curiously recurring template pattern) to forward to a target type derived from ForwardSubscribe<..>
    */
    template<typename Data, typename Target >
    class ForwardSubscribe : public SubscribeTo<Data>
    {
    public:
        /** Create subscription to the data type
         * @param typeName  Unique name given to the serialised data entry @note Replaces compiler generated name which is not portable
         */
        ForwardSubscribe( const char* typeName = nullptr )
            : SubscribeTo<Data>(typeName)
        {}

        /** Empty
         */
        virtual ~ForwardSubscribe()
        {}

        /** Recievs subscribed data and forward to target object
         * @param data  Data to forward
         */
        virtual void receive( const Data& data )
        {
            static_cast<Target*>(this)->forward( data );
        }
    };
}

#endif
