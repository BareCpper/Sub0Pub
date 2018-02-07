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
#include <algorithm>
#include <cassert>
#include <cstdint>

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


	/** @tparam[in] cMessageTrace Enable logging for broker events */
	template< const bool cMessageTrace>
	struct BrokerDetailT
	{
		template<typename Data>
		static void onSubscription( const Broker<Data>& broker, SubscribeTo<Data>* subscriber, const uint32_t count, const uint32_t capacity )
		{
			assert( subscriber != nullptr );
			assert( count < capacity );
			if ( cMessageTrace )
				std::cout << "[Sub0Pub] New Subscription " << *subscriber << " for Broker<" <<  typeid(Data).name () << ">{" << broker << '}' << std::endl;
		}

		template<typename Data>
		static void onPublication( PublishTo<Data>* publisher, const Broker<Data>& broker, const uint32_t count, const uint32_t capacity )
		{
			assert( publisher != nullptr );
			assert( count < capacity );
			if ( cMessageTrace )
				std::cout << "[Sub0Pub] New Publication " << *publisher << " for Broker<" <<  typeid(Data).name () << ">{" << broker << '}' << std::endl;
		}

		template<typename Data>
		static void onPublish( const PublishTo<Data>& publisher, const Data& data )
		{
			if ( cMessageTrace )
				std::cout << "[Sub0Pub] Published " << publisher 
					<< " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << typeid(Data).name () << ']' << std::endl;
		}

		template<typename Data>
		static void onReceive( SubscribeTo<Data>* subscriber, const Data& data )
		{
			assert( subscriber != nullptr );
			if ( cMessageTrace )
				std::cout << "[Sub0Pub] Received " << *subscriber
					<< " {_data_todo_}"/** @todo Data serialize: << data*/ << '[' << typeid(Data).name () << ']' << std::endl;
		}
	};
	typedef BrokerDetailT<true> BrokerDetail;
    
    /** Base type for an object that subscribes to some strong-typed Data
     */
    template< typename Data >
    class SubscribeTo
    {
    public:
        /** Registers the subscriber within the broker framework
         */
        SubscribeTo() 
#pragma warning(suppress: 4355)    ///< warning C4355: 'this' : used in base member initializer list 
        : broker_( this ) 
        {}
    
        /** Receive published Data
         * @remark Data is published from PublishTo<Data>::publish
         */
        virtual void receive( const Data & data ) = 0;

		virtual const char* name() const { return typeid(Data).name(); }

		friend std::ostream& operator<< ( std::ostream& stream, const SubscribeTo<Data>& subscriber )
		{ return stream << subscriber.name() << '{' << (void*)&subscriber << '}'; }

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
         */
        PublishTo()   
  #pragma warning(suppress: 4355)    ///< warning C4355: 'this' : used in base member initializer list 
        : broker_( this ) 
        {}
    
        /** Publish data to subscribers 
         * @remark Data is received by SubscribeTo<Data>::receive
         */
        void publish( const Data& data ) const
        { 
			BrokerDetail::onPublish( *this, data );
			broker_.publish (data); 
		}
        
		virtual const char* name() const { return typeid(Data).name(); }

		friend std::ostream& operator<< ( std::ostream& stream, const PublishTo<Data>& publisher )
		{ return stream << publisher.name() << '{' << (void*)&publisher << '}'; }

    private:
        Broker<Data> broker_; ///< MonoState broker instance to manage publish-subscribe connections
    };
    
    /** Broker manages publisher-subscriber connection for a data-type
     * @todo Cross-module support
     */
    template< typename Data > 
    class Broker
    {
    public:
        static const uint32_t cMaxSubscriptions = 5U;
    
    public:
        Broker ( SubscribeTo<Data>* subscriber )
        {
			BrokerDetail::onSubscription( *this, subscriber, state_.subscriptionCount, cMaxSubscriptions );
            state_.subscriptions[state_.subscriptionCount++] = subscriber;     
        }

        Broker ( PublishTo<Data>* publisher )
        {
			BrokerDetail::onPublication( publisher, *this, 0, 1/* @note No limit at present */ );
            // Do nothing for now...
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
    
    private:
        /** Object state as monotonic object shared by all instances
        */
        struct State
        {
            uint32_t subscriptionCount; ///< Count of subscriptions_
            SubscribeTo<Data>* subscriptions[cMaxSubscriptions];	///< Subscription table @todo More flexible count-support
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
}
