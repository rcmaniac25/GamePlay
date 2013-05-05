#ifndef VR_DEVICE_H_
#define VR_DEVICE_H_

namespace gameplay
{

/**
 * A virtual reality device.
 */
class VRDevice
{
	friend class HMD;
	friend class VRLatencyTester;
	friend class VRSensor;
	friend class VRController;

public:

	/**
     *  VR events.
     */
    enum VREvent
    {
        CONNECTED_EVENT,
        DISCONNECTED_EVENT,

        SENSOR_EVENT,

		LATENCY_TESTER_SAMPLES_EVENT,
		LATENCY_TESTER_COLOR_DETECTED_EVENT,
		LATENCY_TESTER_STARTED_EVENT,
		LATENCY_TESTER_BUTTON_EVENT
    };

	/**
     *  VR types.
     */
    enum VRTypes
    {
        HMD_TYPE,
		SENSOR_TYPE,
		LATENCY_TESTER_TYPE,

		ALL_TYPES
    };

	/**
	 * Get if VR is supported by GamePlay.
	 * 
	 * If VR is supported by GamePlay, then any compatiable
	 * VR device will show up within GamePlay for use.
	 * 
	 * @return true if VR support is built in.
	 */
	static bool isVRSupported();

	/**
	 * Get the device type.
	 * 
	 * @return The VR device type.
	 */
	virtual VRTypes getType() const = 0;

	/**
	 * Get the device hardware version.
	 * 
	 * @return The device hardware version.
	 */
	unsigned int getVersion() const;

	/**
	 * Get the device name.
	 * 
	 * @return The device name.
	 */
	const char* getProductName() const;

	/**
	 * Get the device manufacturer.
	 * 
	 * @return The device manufacturer.
	 */
	const char* getManufacturer() const;

private:

    VRDevice(void* handle);
	virtual ~VRDevice();

	VRDevice(const VRDevice& copy);
    VRDevice& operator=(const VRDevice&);

	void* _deviceHandle;
	unsigned int _version;
	std::string _name;
	std::string _manuf;

};

}

#endif
