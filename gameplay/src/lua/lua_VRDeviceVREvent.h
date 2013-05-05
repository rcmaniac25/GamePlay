#ifndef LUA_VRDEVICEVREVENT_H_
#define LUA_VRDEVICEVREVENT_H_

#include "VRDevice.h"

namespace gameplay
{

// Lua bindings for enum conversion functions for VRDevice::VREvent.
VRDevice::VREvent lua_enumFromString_VRDeviceVREvent(const char* s);
const char* lua_stringFromEnum_VRDeviceVREvent(VRDevice::VREvent e);

}

#endif
