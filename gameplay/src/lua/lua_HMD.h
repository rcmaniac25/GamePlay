#ifndef LUA_HMD_H_
#define LUA_HMD_H_

namespace gameplay
{

// Lua bindings for HMD.
int lua_HMD_getManufacturer(lua_State* state);
int lua_HMD_getProductName(lua_State* state);
int lua_HMD_getRenderState(lua_State* state);
int lua_HMD_getType(lua_State* state);
int lua_HMD_getVersion(lua_State* state);
int lua_HMD_static_isVRSupported(lua_State* state);

void luaRegister_HMD();

}

#endif
