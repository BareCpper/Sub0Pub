#pragma once
#include <iostream>

#include "sub0pub.hpp"


class A : public sub0::PublishTo<float>
		, public sub0::PublishTo<int>
{
public:
	virtual const char* name() const override { return "class A"; }

	void doIt()
	{
		const float floatData = 1.019F;
		std::cout << "A sent float : " << floatData << std::endl;
		sub0::publish( this, floatData );

		const int intData = 2;
		std::cout << "A sent int : " << intData << std::endl;
		sub0::publish( this, intData );
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
		std::cout << "B received float : " << data << std::endl;
		total += data;
	}
	virtual void receive( const int& data )
	{
		std::cout << "B received int : " << data << std::endl;
		total += data;
	}
};
