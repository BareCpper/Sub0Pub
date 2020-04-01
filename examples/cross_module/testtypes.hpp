#pragma once
#include <iostream>

#include "sub0pub/sub0pub.hpp"


class A : public sub0::Publish<float>
		, public sub0::Publish<int>
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


class D : public sub0::StreamDeserialiser<sub0::BinarySerializer>
        , public sub0::ForwardPublish<float,D>
        , public sub0::ForwardPublish<int,D>
{
public:
    D()
        : sub0::StreamDeserialiser<>( std::cin )
    {
    }
};


float total = 0.0F;

class B : public sub0::Subscribe<float>
	, public sub0::Subscribe<int>
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

class C : public sub0::StreamSerialiser<sub0::BinarySerializer>
        , public sub0::ForwardSubscribe<float,C>
        , public sub0::ForwardSubscribe<int,C>
{
public:
    C() : sub0::StreamSerialiser<sub0::BinarySerializer>( std::cout )
    {}


    template<typename Data>
    void forward( const Data& data )
    {
        std::cout << "Serialised " << data << ":\n";
        sub0::StreamSerialiser<sub0::BinarySerializer>::forward(data);
    }
};
