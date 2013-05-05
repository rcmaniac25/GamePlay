#include "Base.h"
#include "VRSensor.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

VRSensor::VRSensor(void* handle) : VRDevice(handle)
{
}

VRSensor::~VRSensor()
{
}

VRDevice::VRTypes VRSensor::getType() const
{
	return VRDevice::SENSOR_TYPE;
}

}
