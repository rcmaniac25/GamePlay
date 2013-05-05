#ifndef LUA_VRSENSOR_H_
#define LUA_VRSENSOR_H_

namespace gameplay
{

// Lua bindings for VRSensor.
int lua_VRSensor_getManufacturer(lua_State* state);
int lua_VRSensor_getProductName(lua_State* state);
int lua_VRSensor_getType(lua_State* state);
int lua_VRSensor_getVersion(lua_State* state);
int lua_VRSensor_static_isVRSupported(lua_State* state);

void luaRegister_VRSensor();

}

#endif
