#include <iostream>

#include "testtypes.hpp"
#include "Publisher/publisher.h"

int main ()
{
	B b[2U]; //< N subscribers
	
    C c; ///< forwarder

	doPublisher();

	std::cout << "Total : " << total << std::endl;
	return int(total);
}

