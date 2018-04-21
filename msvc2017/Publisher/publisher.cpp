#include "../testtypes.hpp"
#include "publisher.h"


void doPublisher()
{
	A a;

	// Do work
	for ( uint32_t i = 0U; i < 3U; ++i )
	{
		a.doIt ();
	}
}