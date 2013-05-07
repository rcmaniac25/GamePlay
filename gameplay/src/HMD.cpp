#include "Base.h"
#include "HMD.h"
#include "Camera.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

//HMDCamera
HMD::HMDCamera::HMDCamera(Camera* c, bool e) : cam(c), enabled(e)
{
	if(c)
	{
		c->addRef();
	}
}

HMD::HMDCamera::~HMDCamera()
{
	SAFE_RELEASE(cam);
}

//HMD
HMD::HMD(void* handle) : VRDevice(handle), _rendState(HMD::NOT_RENDERING), _cameras()
{
}

HMD::~HMD()
{
}

VRDevice::VRTypes HMD::getType() const
{
	return VRDevice::HMD_TYPE;
}

HMD::RenderState HMD::getRenderState() const
{
	return _rendState;
}

HMD::RenderMode HMD::getRenderMode() const
{
	//TODO
	return HMD::MONO_MODE;
}

}
