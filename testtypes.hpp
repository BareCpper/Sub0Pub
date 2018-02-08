#pragma once
#include <iostream>

#include "sub0pub.hpp"


class A : public sub0::PublishTo<float>
		, public sub0::PublishTo<int>
{
public:
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

class C : public sub0::StreamSerializer<sub0::BinaryProtocol>
        , public sub0::ForwardSubscribe<float,C>
        , public sub0::ForwardSubscribe<int,C>
{
public:
    C() : sub0::StreamSerializer<sub0::BinaryProtocol>( std::cout )
    {}


    template<typename Data>
    void forward( const Data& data )
    {
        std::cout << "Serialised " << data << ":\n";
        sub0::StreamSerializer<sub0::BinaryProtocol>::forward(data);
    }
};
