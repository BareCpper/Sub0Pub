#pragma once

#include "sub0pub/sub0pub.hpp"

#if Sub0Pub_CrossModule_Publisher_EXPORTS
#define DLLEXPORT __declspec(dllexport)   
#define DLLEXTERN
#else
#define DLLEXPORT __declspec(dllimport)
#define DLLEXTERN extern
#endif


// @note Exporting the broker is critical for the instance to become shared across DLL module boundaries
DLLEXTERN template class DLLEXPORT sub0::Broker<float>;
DLLEXTERN template class DLLEXPORT sub0::Broker<int>;

#include "sub0pub/sub0pub.hpp"

DLLEXPORT void doPublisher();