#include "Base.h"
#include "ScriptController.h"
#include "lua_HMD.h"
#include "Base.h"
#include "HMD.h"
#include "VRDevice.h"
#include "lua_HMDRenderState.h"
#include "lua_VRDeviceVRTypes.h"

namespace gameplay
{

void luaRegister_HMD()
{
    const luaL_Reg lua_members[] = 
    {
        {"getManufacturer", lua_HMD_getManufacturer},
        {"getProductName", lua_HMD_getProductName},
        {"getRenderState", lua_HMD_getRenderState},
        {"getType", lua_HMD_getType},
        {"getVersion", lua_HMD_getVersion},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"isVRSupported", lua_HMD_static_isVRSupported},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    gameplay::ScriptUtil::registerClass("HMD", lua_members, NULL, NULL, lua_statics, scopePath);
}

static HMD* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "HMD");
    luaL_argcheck(state, userdata != NULL, 1, "'HMD' expected.");
    return (HMD*)((gameplay::ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_HMD_getManufacturer(lua_State* state)
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
                HMD* instance = getInstance(state);
                const char* result = instance->getManufacturer();

                // Push the return value onto the stack.
                lua_pushstring(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_HMD_getManufacturer - Failed to match the given parameters to a valid function signature.");
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

int lua_HMD_getProductName(lua_State* state)
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
                HMD* instance = getInstance(state);
                const char* result = instance->getProductName();

                // Push the return value onto the stack.
                lua_pushstring(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_HMD_getProductName - Failed to match the given parameters to a valid function signature.");
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

int lua_HMD_getRenderState(lua_State* state)
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
                HMD* instance = getInstance(state);
                HMD::RenderState result = instance->getRenderState();

                // Push the return value onto the stack.
                lua_pushstring(state, lua_stringFromEnum_HMDRenderState(result));

                return 1;
            }

            lua_pushstring(state, "lua_HMD_getRenderState - Failed to match the given parameters to a valid function signature.");
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

int lua_HMD_getType(lua_State* state)
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
                HMD* instance = getInstance(state);
                VRDevice::VRTypes result = instance->getType();

                // Push the return value onto the stack.
                lua_pushstring(state, lua_stringFromEnum_VRDeviceVRTypes(result));

                return 1;
            }

            lua_pushstring(state, "lua_HMD_getType - Failed to match the given parameters to a valid function signature.");
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

int lua_HMD_getVersion(lua_State* state)
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
                HMD* instance = getInstance(state);
                unsigned int result = instance->getVersion();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_HMD_getVersion - Failed to match the given parameters to a valid function signature.");
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

int lua_HMD_static_isVRSupported(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            bool result = HMD::isVRSupported();

            // Push the return value onto the stack.
            lua_pushboolean(state, result);

            return 1;
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 0).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
