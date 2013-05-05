#ifndef VRCONTROLLER_H_
#define VRCONTROLLER_H_

#include "VRDevice.h"

namespace gameplay
{
	class OculusData;

/**
 * The AIController facilitates virtual reality device execution such as 
 * head mounted displays and stero rendering. This class is generally not 
 * interfaced with directly.
 */
class VRController
{
	friend class Game;

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
	 * Called during update to check if any new devices have been added or removed.
	 */
	void pollDevices();

	/**
	 * Called during rendering to determine if rendering should happen twice.
	 */
	bool shouldRenderStereo();

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

	OculusData* _data;

};

}

#endif