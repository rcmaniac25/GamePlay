#ifndef LUA_VRLATENCYTESTER_H_
#define LUA_VRLATENCYTESTER_H_

namespace gameplay
{

// Lua bindings for VRLatencyTester.
int lua_VRLatencyTester_getManufacturer(lua_State* state);
int lua_VRLatencyTester_getProductName(lua_State* state);
int lua_VRLatencyTester_getType(lua_State* state);
int lua_VRLatencyTester_getVersion(lua_State* state);
int lua_VRLatencyTester_static_isVRSupported(lua_State* state);

void luaRegister_VRLatencyTester();

}

#endif
