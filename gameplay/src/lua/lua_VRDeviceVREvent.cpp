#include "Base.h"
#include "lua_VRDeviceVREvent.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_VRDeviceVREvent_CONNECTED_EVENT = "CONNECTED_EVENT";
static const char* luaEnumString_VRDeviceVREvent_DISCONNECTED_EVENT = "DISCONNECTED_EVENT";
static const char* luaEnumString_VRDeviceVREvent_SENSOR_EVENT = "SENSOR_EVENT";
static const char* luaEnumString_VRDeviceVREvent_LATENCY_TESTER_SAMPLES_EVENT = "LATENCY_TESTER_SAMPLES_EVENT";
static const char* luaEnumString_VRDeviceVREvent_LATENCY_TESTER_COLOR_DETECTED_EVENT = "LATENCY_TESTER_COLOR_DETECTED_EVENT";
static const char* luaEnumString_VRDeviceVREvent_LATENCY_TESTER_STARTED_EVENT = "LATENCY_TESTER_STARTED_EVENT";
static const char* luaEnumString_VRDeviceVREvent_LATENCY_TESTER_BUTTON_EVENT = "LATENCY_TESTER_BUTTON_EVENT";

VRDevice::VREvent lua_enumFromString_VRDeviceVREvent(const char* s)
{
    if (strcmp(s, luaEnumString_VRDeviceVREvent_CONNECTED_EVENT) == 0)
        return VRDevice::CONNECTED_EVENT;
    if (strcmp(s, luaEnumString_VRDeviceVREvent_DISCONNECTED_EVENT) == 0)
        return VRDevice::DISCONNECTED_EVENT;
    if (strcmp(s, luaEnumString_VRDeviceVREvent_SENSOR_EVENT) == 0)
        return VRDevice::SENSOR_EVENT;
    if (strcmp(s, luaEnumString_VRDeviceVREvent_LATENCY_TESTER_SAMPLES_EVENT) == 0)
        return VRDevice::LATENCY_TESTER_SAMPLES_EVENT;
    if (strcmp(s, luaEnumString_VRDeviceVREvent_LATENCY_TESTER_COLOR_DETECTED_EVENT) == 0)
        return VRDevice::LATENCY_TESTER_COLOR_DETECTED_EVENT;
    if (strcmp(s, luaEnumString_VRDeviceVREvent_LATENCY_TESTER_STARTED_EVENT) == 0)
        return VRDevice::LATENCY_TESTER_STARTED_EVENT;
    if (strcmp(s, luaEnumString_VRDeviceVREvent_LATENCY_TESTER_BUTTON_EVENT) == 0)
        return VRDevice::LATENCY_TESTER_BUTTON_EVENT;
    return VRDevice::CONNECTED_EVENT;
}

const char* lua_stringFromEnum_VRDeviceVREvent(VRDevice::VREvent e)
{
    if (e == VRDevice::CONNECTED_EVENT)
        return luaEnumString_VRDeviceVREvent_CONNECTED_EVENT;
    if (e == VRDevice::DISCONNECTED_EVENT)
        return luaEnumString_VRDeviceVREvent_DISCONNECTED_EVENT;
    if (e == VRDevice::SENSOR_EVENT)
        return luaEnumString_VRDeviceVREvent_SENSOR_EVENT;
    if (e == VRDevice::LATENCY_TESTER_SAMPLES_EVENT)
        return luaEnumString_VRDeviceVREvent_LATENCY_TESTER_SAMPLES_EVENT;
    if (e == VRDevice::LATENCY_TESTER_COLOR_DETECTED_EVENT)
        return luaEnumString_VRDeviceVREvent_LATENCY_TESTER_COLOR_DETECTED_EVENT;
    if (e == VRDevice::LATENCY_TESTER_STARTED_EVENT)
        return luaEnumString_VRDeviceVREvent_LATENCY_TESTER_STARTED_EVENT;
    if (e == VRDevice::LATENCY_TESTER_BUTTON_EVENT)
        return luaEnumString_VRDeviceVREvent_LATENCY_TESTER_BUTTON_EVENT;
    return enumStringEmpty;
}

}

