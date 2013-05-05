#ifndef VR_LATENCY_TESTER_H_
#define VR_LATENCY_TESTER_H_

#include "VRDevice.h"

namespace gameplay
{

/**
 * A VR latency tester.
 */
class VRLatencyTester : public VRDevice
{
	friend class VRController;

public:

	/**
     * @see VRDevice::getType
     */
	VRTypes getType() const;

    //TODO

private:

    VRLatencyTester(void* handle);
	~VRLatencyTester();

	VRLatencyTester(const VRLatencyTester& copy);
    VRLatencyTester& operator=(const VRLatencyTester&);

};

}

#endif
