#ifndef HMD_H_
#define HMD_H_

#include "VRDevice.h"

namespace gameplay
{

/**
 * Head-mounted display VR control
 */
class HMD : public VRDevice
{
	friend class VRController;

public:

	/**
     * Render state.
     */
    enum RenderState
    {
        LEFT_EYE_RENDERING,
		RIGHT_EYE_RENDERING,
		MONO_RENDERING,
		NOT_RENDERING
    };

	/**
     * Render mode.
     */
    enum RenderMode
    {
        MONO_MODE,
		STEREO_MODE,
		LEFT_RIGHT_MODE
    };

	/**
     * @see VRDevice::getType
     */
	VRTypes getType() const;

	/**
	 * Get the current render state at the current point in time.
	 * 
	 * @return The current render state.
	 */
	RenderState getRenderState() const;

    //TODO

private:

    HMD(void* handle);
	~HMD();

	HMD(const HMD& copy);
    HMD& operator=(const HMD&);

};

}

#endif
