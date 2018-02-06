#pragma once

#include "../sub0pub.hpp"

#if PUBLISHER_EXPORTS
#define DLLEXPORT __declspec(dllexport)   
#define DLLEXTERN
#else
#define DLLEXPORT __declspec(dllimport)
#define DLLEXTERN extern
#endif


DLLEXTERN template class DLLEXPORT sub0::Broker<float>;
DLLEXTERN template class DLLEXPORT sub0::Broker<int>;


DLLEXPORT void doPublisher();