#include <iostream>

#include "publisher.hpp"
#include "testtypes.hpp"

int main ()
{
	B b[2U]; //< N subscribers
	
    C c; ///< forwarder

	doPublisher();

	std::cout << "Total : " << total << std::endl;
	return int(total);
}

