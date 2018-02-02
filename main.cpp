#include <iostream>

#include "sub0pub.hpp"

class A : public sub0::PublishTo<float>
		, public sub0::PublishTo<int>
{
public:
	virtual const char* name() const override { return "class A"; }

	void doIt()
	{
		PublishTo<float>::publish( 1.019F );
		PublishTo<int>::publish( 2.0F );
	}
};

float total = 0.0F;

class B : public sub0::SubscribeTo<float>
		, public sub0::SubscribeTo<int>
{
public:
	virtual const char* name() const override { return "class B"; }

	virtual void receive( const float& data )
	{
		//std::cout << "B received float : " << data << std::endl;
		total += data;
	}
	virtual void receive( const int& data )
	{
		//std::cout << "B received int : " << data << std::endl;
		total += data;
	}
};

int main ()
{
	A a;
	B b[2U]; //< N subscribers

			 // Do work
	for ( uint32_t i = 0U; i < 3U; ++i )
	{
		a.doIt ();
	}

	std::cout << "Total : " << total << std::endl;
	return int(total);
}

