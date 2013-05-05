#ifndef VR_SENSOR_H_
#define VR_SENSOR_H_

#include "VRDevice.h"

namespace gameplay
{

/**
 * A VR sensor.
 */
class VRSensor : public VRDevice
{
	friend class VRController;

public:

	/**
     * @see VRDevice::getType
     */
	VRTypes getType() const;

    //TODO

private:

    VRSensor(void* handle);
	~VRSensor();

	VRSensor(const VRSensor& copy);
    VRSensor& operator=(const VRSensor&);

};

}

#endif
