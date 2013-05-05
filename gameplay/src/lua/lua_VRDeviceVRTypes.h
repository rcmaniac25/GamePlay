#ifndef LUA_VRDEVICEVRTYPES_H_
#define LUA_VRDEVICEVRTYPES_H_

#include "VRDevice.h"

namespace gameplay
{

// Lua bindings for enum conversion functions for VRDevice::VRTypes.
VRDevice::VRTypes lua_enumFromString_VRDeviceVRTypes(const char* s);
const char* lua_stringFromEnum_VRDeviceVRTypes(VRDevice::VRTypes e);

}

#endif
