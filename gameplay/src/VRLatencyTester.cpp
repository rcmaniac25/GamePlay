#include "Base.h"
#include "VRLatencyTester.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

VRLatencyTester::VRLatencyTester(void* handle) : VRDevice(handle)
{
}

VRLatencyTester::~VRLatencyTester()
{
}

VRDevice::VRTypes VRLatencyTester::getType() const
{
	return VRDevice::LATENCY_TESTER_TYPE;
}

}
