// Shadow include for the embedded-size spike: `#include "sub0pub/sub0pub.hpp"` resolves to the lever-gated copy,
// so the unmodified test suite (tests/test_pubsub.cpp) can be recompiled against it. See docs/design/spikes/embedded_size.md.
#include "../../../../collapse/sub0pub_variants/sub0pub_spike.hpp"
