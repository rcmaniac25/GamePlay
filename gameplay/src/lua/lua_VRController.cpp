#include "Base.h"
#include "ScriptController.h"
#include "lua_VRController.h"
#include "Base.h"
#include "Game.h"
#include "HMD.h"
#include "VRController.h"
#include "VRLatencyTester.h"
#include "VRSensor.h"

namespace gameplay
{

void luaRegister_VRController()
{
    const luaL_Reg lua_members[] = 
    {
        {"disableAllEvents", lua_VRController_disableAllEvents},
        {NULL, NULL}
    };
    const luaL_Reg* lua_statics = NULL;
    std::vector<std::string> scopePath;

    gameplay::ScriptUtil::registerClass("VRController", lua_members, NULL, NULL, lua_statics, scopePath);
}

static VRController* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "VRController");
    luaL_argcheck(state, userdata != NULL, 1, "'VRController' expected.");
    return (VRController*)((gameplay::ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_VRController_disableAllEvents(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                VRController* instance = getInstance(state);
                instance->disableAllEvents();
                
                return 0;
            }

            lua_pushstring(state, "lua_VRController_disableAllEvents - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
