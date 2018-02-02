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

namespace sub0 
{
    /** Broker manages publisher-subscriber connection for a data-type
     */
    template< typename Data >
    class Broker;
    
    /** Base type for an object that subscribes to some strong-typed Data
     */
    template< typename Data >
    class SubscribeTo
    {
    public:
        /** Registers the subscriber within the broker framework
         */
        SubscribeTo() : broker_( this ) {}
    
        /** Receive published Data
         * @remark Data is published from PublishTo<Data>::publish
         */
        virtual void receive( const Data & data ) = 0;
      
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
        PublishTo() : broker_( this ) {}
    
        /** Publish data to subscribers 
         * @remark Data is received by SubscribeTo<Data>::receive
         */
        void publish( const Data& data )
        { broker_.publish (data); }
        
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
            assert( subscriptionCount_ < cMaxSubscriptions );
            std::cout << "Subscription to " << typeid(Data).name () << " created" << std::endl;
            subscriptions_[subscriptionCount_++] = subscriber;        
            /// @todo subscription overflow checks
        }
        Broker ( PublishTo<Data>* publisher )
        {
            std::cout << "Publication to " << typeid(Data).name () << " created" << std::endl;
            // Do nothing for now...
        }
    
        void publish (const Data & data)
        {
            const uint32_t subscriptionCount = std::min(subscriptionCount_,cMaxSubscriptions);
            for ( uint32_t iSubscription = 0U; iSubscription < subscriptionCount; ++iSubscription )
            {
                subscriptions_[iSubscription]->receive(data);
            }
        }
    
    private:
        static uint32_t subscriptionCount_; ///< Count of subscriptions_
        static SubscribeTo<Data>* subscriptions_[cMaxSubscriptions];	///< MonoState subscription table @todo support multiple subscription
    };
    
    template<typename Data>  
    uint32_t Broker<Data>::subscriptionCount_ = 0U;
    
    template<typename Data>  
    SubscribeTo<Data>* Broker<Data>::subscriptions_[Broker<Data>::cMaxSubscriptions] = {nullptr};
}
