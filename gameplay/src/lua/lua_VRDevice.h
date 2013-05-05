#ifndef LUA_VRDEVICE_H_
#define LUA_VRDEVICE_H_

namespace gameplay
{

// Lua bindings for VRDevice.
int lua_VRDevice_getManufacturer(lua_State* state);
int lua_VRDevice_getProductName(lua_State* state);
int lua_VRDevice_getType(lua_State* state);
int lua_VRDevice_getVersion(lua_State* state);
int lua_VRDevice_static_isVRSupported(lua_State* state);

void luaRegister_VRDevice();

}

#endif
