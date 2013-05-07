#ifndef LUA_VRLATENCYTESTER_H_
#define LUA_VRLATENCYTESTER_H_

namespace gameplay
{

// Lua bindings for VRLatencyTester.
int lua_VRLatencyTester_getCalibrate(lua_State* state);
int lua_VRLatencyTester_getConfigurationSendSamples(lua_State* state);
int lua_VRLatencyTester_getConfigurationThreshold(lua_State* state);
int lua_VRLatencyTester_getEventsEnabled(lua_State* state);
int lua_VRLatencyTester_getManufacturer(lua_State* state);
int lua_VRLatencyTester_getProductName(lua_State* state);
int lua_VRLatencyTester_getType(lua_State* state);
int lua_VRLatencyTester_getUtilityResults(lua_State* state);
int lua_VRLatencyTester_getVersion(lua_State* state);
int lua_VRLatencyTester_setCalibrate(lua_State* state);
int lua_VRLatencyTester_setConfiguration(lua_State* state);
int lua_VRLatencyTester_setDisplay(lua_State* state);
int lua_VRLatencyTester_setEventsEnabled(lua_State* state);
int lua_VRLatencyTester_setUtilityDisplayScreenColor(lua_State* state);
int lua_VRLatencyTester_startTest(lua_State* state);
int lua_VRLatencyTester_static_isVRSupported(lua_State* state);
int lua_VRLatencyTester_utilityProcessInputs(lua_State* state);

void luaRegister_VRLatencyTester();

}

#endif
