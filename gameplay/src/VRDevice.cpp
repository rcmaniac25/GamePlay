#include "Base.h"
#include "VRDevice.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

VRDevice::VRDevice(void* handle) : _deviceHandle(handle)
{
#ifdef USE_OCULUS
	if(handle)
	{
		OVR::DeviceInfo info;
		OVR::DeviceBase* dev = static_cast<OVR::DeviceBase*>(handle);
		dev->GetDeviceInfo(&info);
		_version = info.Version;
		_name = info.ProductName;
		_manuf = info.Manufacturer;
	}
#endif
}

VRDevice::~VRDevice()
{
	_deviceHandle = NULL;
}

bool VRDevice::isVRSupported()
{
#ifdef USE_OCULUS
	return true;
#else
	return false;
#endif
}

unsigned int VRDevice::getVersion() const
{
	return _version;
}

const char* VRDevice::getProductName() const
{
	return _name.c_str();
}

const char* VRDevice::getManufacturer() const
{
	return _manuf.c_str();
}

}