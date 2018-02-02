/// @copyright  2018 Craig HutchinsonAll Rights Reserved

// ######################################################
// ################# Sub0Pub API Code ###################
// ######################################################

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
