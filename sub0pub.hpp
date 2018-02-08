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
*/
#ifndef CROG_SUB0PUB_HPP
#define CROG_SUB0PUB_HPP

#include <algorithm>
#include <cassert> // assert
#include <cstdint> // uint32_t
#include <iostream> // std::cout
#include <functional> // std::hash @todo Remove as C++11

/** Define SUB0PUB_TRACE=true to enable message logging to std::cout for event trace, SUB0PUB_TRACE=false to disable SUB0PUB_TRACE={empty} for default 
*/
#ifndef SUB0PUB_TRACE
#define SUB0PUB_TRACE
#endif

namespace sub0 
{
    /** Broker manages publisher-subscriber connection for a data-type
     */
    template< typename Data >
    class Broker;
	template< typename Data >
	class SubscribeTo;
	template< typename Data >
	class PublishTo;
    
    /** Create 4byte packed value at compile time
     */
    template <const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d>
    struct FourCC
    {
        static const uint32_t value = (((((d << 8) | c) << 8) | b) << 8) | a;
    };

	/** @tparam[in] cMessageTrace Enable logging for broker events */
	template< const bool cMessageTrace = false>
	struct BrokerDetailT
	{
		template<typename Data>
		static void onSubscription( const Broker<Data>& broker, SubscribeTo<Data>* subscriber, const uint32_t count, const uint32_t capacity )
		{
			assert( subscriber != nullptr );
			assert( count < capacity );
			if ( cMessageTrace )
            {
				std::cout << "[Sub0Pub] New Subscription " << *subscriber << " for Broker<" <<  typeid(Data).name () << ">{" << broker << '}' << std::endl;
            }
		}

		template<typename Data>
		static void onPublication( PublishTo<Data>* publisher, const Broker<Data>& broker, const uint32_t count, const uint32_t capacity )
		{
			assert( publisher != nullptr );
			assert( count < capacity );
			if ( cMessageTrace )
            {
				std::cout << "[Sub0Pub] New Publication " << *publisher << " for Broker<" <<  typeid(Data).name () << ">{" << broker << '}' << std::endl;
            }
		}

		template<typename Data>
		static void onPublish( const PublishTo<Data>& publisher, const Data& data )
		{
			if ( cMessageTrace )
            {
				std::cout << "[Sub0Pub] Published " << publisher 
					<< " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << typeid(Data).name () << ']' << std::endl;
            }
		}

		template<typename Data>
		static void onReceive( SubscribeTo<Data>* subscriber, const Data& data )
		{
			assert( subscriber != nullptr );
			if ( cMessageTrace )
            {
				std::cout << "[Sub0Pub] Received " << *subscriber
					<< " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << typeid(Data).name () << ']' << std::endl;
            }
		}
	};

    /** Broker detail type
     */
	typedef BrokerDetailT<SUB0PUB_TRACE> BrokerDetail;
    
#pragma warning(push)
#pragma warning(disable:4355) ///< warning C4355: 'this' : used in base member initializer list

    /** Base type for an object that subscribes to some strong-typed Data
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
    
        /** Receive published Data
         * @remark Data is published from PublishTo<Data>::publish
         */
        virtual void receive( const Data & data ) = 0;
        
        /** get broker data-name
        */
		const char* typeName() const
        { return broker_.typeName(); }

		friend std::ostream& operator<< ( std::ostream& stream, const SubscribeTo<Data>& subscriber )
		{ return stream << subscriber.typeName() << '{' << (void*)&subscriber << '}'; }

    private:
        Broker<Data> broker_; ///< MonoState broker instance to manage publish-subscribe connections
    };
    
    /** Base type for an object that publishes to some strong-typed Data
     */
    template< typename Data >
    class PublishTo
    {
    public:
        /** Registers the publisher within the broker framework
         * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names will be used.
         */
        PublishTo( const char* typeName = nullptr )   
        : broker_( this, typeName ) 
        {}
    
        /** Publish data to subscribers 
         * @remark Data is received by SubscribeTo<Data>::receive
         */
        void publish( const Data& data ) const
        { 
			BrokerDetail::onPublish( *this, data );
			broker_.publish (data); 
		}
        
        /** get broker data-name
        */
		const char* typeName() const
        { return broker_.typeName(); }

		friend std::ostream& operator<< ( std::ostream& stream, const PublishTo<Data>& publisher )
		{ return stream << publisher.typeName() << '{' << (void*)&publisher << '}'; }

    private:
        Broker<Data> broker_; ///< MonoState broker instance to manage publish-subscribe connections
    };
    
    /** Hash a string using djb2 hash
    * @todo Implement as compile time with name
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

#pragma warning(pop)

    /** Broker manages publisher-subscriber connection for a data-type
     * @todo Cross-module support
     */
    template< typename Data > 
    class Broker
    {
    public:
        static const uint32_t cMaxSubscriptions = 5U;
    
    public:
        /**
        * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names will be used.
        */
        Broker ( SubscribeTo<Data>* subscriber, const char* typeName = nullptr )
        {
			BrokerDetail::onSubscription( *this, subscriber, state_.subscriptionCount, cMaxSubscriptions );
            setDataName(typeName);
            state_.subscriptions[state_.subscriptionCount++] = subscriber;     
        }
        
        /**
        * @param[in] typeName Optional unique data name given to data for inter-process signalling. @warning If not supplied non-portable compiler generated names will be used.
        */
        Broker ( PublishTo<Data>* publisher, const char* typeName = nullptr )
        {
			BrokerDetail::onPublication( publisher, *this, 0, 1/* @note No limit at present */ );
            setDataName(typeName);
            // Do nothing for now...
        }

        void setDataName( const char* typeName )
        {
            if ( typeName == nullptr )
                return;
            
            // @todo use BrokerDetail and handle if a subscriber uses a different name better
            assert( (state_.typeName==nullptr) || (strcmp(state_.typeName,typeName)==0) ); 
            state_.typeName = typeName;            
            state_.typeId = sub0::hash( state_.typeName ); // Cachhe hash result

        }
    
        void publish (const Data & data) const
        {
            const uint32_t subscriptionCount = std::min( state_.subscriptionCount, cMaxSubscriptions );
            for ( uint32_t iSubscription = 0U; iSubscription < subscriptionCount; ++iSubscription )
            {
                SubscribeTo<Data>* subscription = state_.subscriptions[iSubscription];
				BrokerDetail::onReceive( subscription, data );
                subscription->receive(data);
            }
        }

        /** Prints address of monotonic state
        */
		friend std::ostream& operator<< ( std::ostream& stream, const Broker<Data>& broker )
		{ return stream << (void*)&broker.state_; }
            
        /** Unique identifer index for inter-process binary connections
        */
        static uint32_t typeId()
        { return state_.typeId; }

        /** Unique identifer name for inter-process text connections
        */
        static const char* typeName()
        { return state_.typeName != nullptr ? state_.typeName : typeid(Data).name(); }

    private:
        /** Object state as monotonic object shared by all instances
        */
        struct State
        {
            uint32_t subscriptionCount; ///< Count of subscriptions_
            SubscribeTo<Data>* subscriptions[cMaxSubscriptions];	///< Subscription table @todo More flexible count-support
            uint32_t typeId; ///< Type identifer index or name hash
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
    */
    struct BinaryProtocol
    {
        /** Header containing signal type information
        */
        struct Header
        {
            static const uint32_t cMagic = FourCC<'S','U','B','0'>::value; //< Magic number to identify Sub0 network protocol packets 

            uint32_t magic; ///< FourCC identifier containing 'SUB0'
            uint32_t typeId; ///< Data type identifier @note The Id may be user specified for inter-process 
            uint32_t dataBytes; ///< Count of bytes
        };

        template<typename Data>
        static std::ostream& writeHeader( std::ostream& stream, const Data& data )
        { 
            Header header = { Header::cMagic
                             , Broker<Data>::typeId()
                             , sizeof(data) };
            return stream.write( reinterpret_cast<const char*>(&header), sizeof(header) ); 
        }

        /** Write data payload as binary
        */
        template<typename Data>
        static std::ostream& writePayload( std::ostream& stream, const Data& data )
        { return stream.write( reinterpret_cast<const char*>(&data), sizeof(data) ); }
    };

    /** Serialises Sub0Pub data into a target stream pobject 
    * @remark Implements a basic inter process transfer protocol
    */
    template< typename Protocol = BinaryProtocol >
    class StreamSerializer
    {
    public:
        StreamSerializer( std::ostream& stream ) 
        : stream_(stream) 
        {}

        virtual ~StreamSerializer()
        {}

        template<typename Data>
        void forward( const Data& data )
        {
            Protocol::writeHeader( stream_, data );
            Protocol::writePayload( stream_, data );
            stream_ << '\n';
            stream_.flush(); ///< @todo Temp?
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
        ForwardSubscribe( const char* typeName = nullptr )
            : SubscribeTo<Data>(typeName)
        {}
        virtual void receive( const Data& data )
        {
            static_cast<Target*>(this)->forward( data );
        }
    };
}

#endif
