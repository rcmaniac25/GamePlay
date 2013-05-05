#include "Base.h"
#include "HMD.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

HMD::HMD(void* handle) : VRDevice(handle)
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
#ifdef USE_OCULUS
	//TODO
	return HMD::NOT_RENDERING;
#else
	return HMD::NOT_RENDERING;
#endif
}

}
