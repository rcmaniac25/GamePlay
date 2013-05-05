#include "Base.h"
#include "lua_VRDeviceVRTypes.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_VRDeviceVRTypes_HMD_TYPE = "HMD_TYPE";
static const char* luaEnumString_VRDeviceVRTypes_SENSOR_TYPE = "SENSOR_TYPE";
static const char* luaEnumString_VRDeviceVRTypes_LATENCY_TESTER_TYPE = "LATENCY_TESTER_TYPE";
static const char* luaEnumString_VRDeviceVRTypes_ALL_TYPES = "ALL_TYPES";

VRDevice::VRTypes lua_enumFromString_VRDeviceVRTypes(const char* s)
{
    if (strcmp(s, luaEnumString_VRDeviceVRTypes_HMD_TYPE) == 0)
        return VRDevice::HMD_TYPE;
    if (strcmp(s, luaEnumString_VRDeviceVRTypes_SENSOR_TYPE) == 0)
        return VRDevice::SENSOR_TYPE;
    if (strcmp(s, luaEnumString_VRDeviceVRTypes_LATENCY_TESTER_TYPE) == 0)
        return VRDevice::LATENCY_TESTER_TYPE;
    if (strcmp(s, luaEnumString_VRDeviceVRTypes_ALL_TYPES) == 0)
        return VRDevice::ALL_TYPES;
    return VRDevice::HMD_TYPE;
}

const char* lua_stringFromEnum_VRDeviceVRTypes(VRDevice::VRTypes e)
{
    if (e == VRDevice::HMD_TYPE)
        return luaEnumString_VRDeviceVRTypes_HMD_TYPE;
    if (e == VRDevice::SENSOR_TYPE)
        return luaEnumString_VRDeviceVRTypes_SENSOR_TYPE;
    if (e == VRDevice::LATENCY_TESTER_TYPE)
        return luaEnumString_VRDeviceVRTypes_LATENCY_TESTER_TYPE;
    if (e == VRDevice::ALL_TYPES)
        return luaEnumString_VRDeviceVRTypes_ALL_TYPES;
    return enumStringEmpty;
}

}

