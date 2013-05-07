#ifndef VRCONTROLLER_H_
#define VRCONTROLLER_H_

#include "VRDevice.h"

namespace gameplay
{
	class OculusData;
	class VRHandler;

/**
 * The AIController facilitates virtual reality device execution such as 
 * head mounted displays and stero rendering. This class is generally not 
 * interfaced with directly.
 */
class VRController
{
	friend class Game;
	friend class VRLatencyTester;
	friend class OculusLatencyHandler;

public:

	/**
     * Disables all VRDevice events.
	 * 
	 * Some VRDevices can send events to Game.vrEvent. This function
	 * disables all those devices, requiring that they manually be
	 * turned back on if events are to be sent again.
	 * 
	 * The only events that will continue to be sent and recieved
	 * are the connected/disconnected events.
     */
	void disableAllEvents();

private:
	/**
     * Constructor.
     */
    VRController();

    /**
     * Destructor.
     */
    ~VRController();

    /**
     * Hidden copy constructor.
     */
    VRController(const VRController&);

    /**
     * Hidden copy assignment operator.
     */
    VRController& operator=(const VRController&);

	/**
     * Called during startup to initialize the VRController.
     */
    void initialize();

    /**
     * Called during shutdown to finalize the VRController.
     */
    void finalize();

	/**
	 * Called during update to check if any new devices have been added or removed and do maintenance work.
	 */
	void pollDevices();

	/**
	 * Called during rendering to determine if how many times rendering should occur. Use with prepareRender and finalizeRender.
	 */
	unsigned int renderIterationCount();

	/**
	 * Called prior to rendering to make sure that rendering is done properly.
	 */
	void prepareRender(unsigned int index);

	/**
	 * Called after rendering to perform any necessary distortion/correction.
	 */
	void finalizeRender(unsigned int index);

	void lockMessages();

	void releaseMessages();

	unsigned int getDeviceCount(VRDevice::VRTypes type = VRDevice::ALL_TYPES) const;

	VRDevice* getDevice(unsigned int, VRDevice::VRTypes type = VRDevice::ALL_TYPES) const;

	VRHandler* getLatencyHandler();

	//Static
	static void* getDeviceHandle(VRDevice* device);

	//Data
	OculusData* _data;

};

/**
 * For handling VR devices.
 * 
 * For internal use only
 * 
 * @script{ignore}
 */
class VRHandler
{
public:
	/**
	 * Add a VR device
	 * 
	 * For internal use only
	 * 
	 * @param device Device to add.
	 * @script{ignore}
	 */
	virtual void AddDevice(VRDevice* device) = 0;
	/**
	 * Remove a VR device
	 * 
	 * For internal use only
	 * 
	 * @param device Device to remove.
	 * @script{ignore}
	 */
	virtual void RemoveDevice(VRDevice* device) = 0;
	/**
	 * Remove all devices.
	 * 
	 * For internal use only
	 * 
	 * @script{ignore}
	 */
	virtual void RemoveAllDevices() = 0;
};

}

#endif