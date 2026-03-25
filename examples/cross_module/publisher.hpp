#pragma once

#include "sub0pub/sub0pub.hpp"

// Cross-platform shared library export/import macros
#if defined(_WIN32)
    #if Sub0Pub_CrossModule_Publisher_EXPORTS
        #define SUB0PUB_EXAMPLE_API __declspec(dllexport)
        #define SUB0PUB_EXAMPLE_EXTERN
    #else
        #define SUB0PUB_EXAMPLE_API __declspec(dllimport)
        #define SUB0PUB_EXAMPLE_EXTERN extern
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define SUB0PUB_EXAMPLE_API __attribute__((visibility("default")))
    #define SUB0PUB_EXAMPLE_EXTERN
#else
    #define SUB0PUB_EXAMPLE_API
    #define SUB0PUB_EXAMPLE_EXTERN
#endif

// Exporting the broker is critical for the instance to become shared across shared library boundaries
SUB0PUB_EXAMPLE_EXTERN template class SUB0PUB_EXAMPLE_API sub0::detail::Broker<float>;
SUB0PUB_EXAMPLE_EXTERN template class SUB0PUB_EXAMPLE_API sub0::detail::Broker<int>;

SUB0PUB_EXAMPLE_API void doPublisher();