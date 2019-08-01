/** Minimal (smallest) example of Sub0Pub
*/

#include "sub0pub/sub0pub.hpp"

typedef  unsigned int Data_t; ///< Port data type we publish and subscribe
Data_t total = 0U; ///< Total of all published data
const Data_t cIncrement = 3141U; ///< Amount total is incremented on each publish of Data_t

/** Class publishes a 'Data_t' signal
*/
class PubInt : public sub0::Publish<Data_t>
{
public:

    /** Publishes the increment value to all subscribers of this type
    */
	void doIt()
	{
		sub0::publish( this, cIncrement );
	}
};

/** Class subscribes to any 'Data_t' signals 
*/
class SubInt : public sub0::Subscribe<Data_t>
{
public:

    /** Receive data published of type 'Data_t' and adds it to the 'total' 
    * @param[in] value  The published value which is added to total as 'total += value'
    */
	virtual void receive( const Data_t& value ) final
	{
		total += value;
	}
};

int main ()
{
    // Publishes the Data_t value
	PubInt publisher; //< Publisher
	
    // Subscriber that does 'total += Data_t'
    SubInt subscriber;

    // PubInt will publish the Data_t value 'cIncrement'
    publisher.doIt();

    // Return result: 
    //   total = cDoItCallCount * (cSubsciberCount * cIncrement)
    return static_cast<int>(total);
}