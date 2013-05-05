#ifndef LUA_VRCONTROLLER_H_
#define LUA_VRCONTROLLER_H_

namespace gameplay
{

// Lua bindings for VRController.
int lua_VRController_disableAllEvents(lua_State* state);

void luaRegister_VRController();

}

#endif
