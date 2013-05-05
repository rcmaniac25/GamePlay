#include "Base.h"
#include "ScriptController.h"
#include "lua_VRSensor.h"
#include "Base.h"
#include "VRDevice.h"
#include "VRSensor.h"
#include "lua_VRDeviceVRTypes.h"

namespace gameplay
{

void luaRegister_VRSensor()
{
    const luaL_Reg lua_members[] = 
    {
        {"getManufacturer", lua_VRSensor_getManufacturer},
        {"getProductName", lua_VRSensor_getProductName},
        {"getType", lua_VRSensor_getType},
        {"getVersion", lua_VRSensor_getVersion},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"isVRSupported", lua_VRSensor_static_isVRSupported},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    gameplay::ScriptUtil::registerClass("VRSensor", lua_members, NULL, NULL, lua_statics, scopePath);
}

static VRSensor* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "VRSensor");
    luaL_argcheck(state, userdata != NULL, 1, "'VRSensor' expected.");
    return (VRSensor*)((gameplay::ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_VRSensor_getManufacturer(lua_State* state)
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
                VRSensor* instance = getInstance(state);
                const char* result = instance->getManufacturer();

                // Push the return value onto the stack.
                lua_pushstring(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_VRSensor_getManufacturer - Failed to match the given parameters to a valid function signature.");
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

int lua_VRSensor_getProductName(lua_State* state)
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
                VRSensor* instance = getInstance(state);
                const char* result = instance->getProductName();

                // Push the return value onto the stack.
                lua_pushstring(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_VRSensor_getProductName - Failed to match the given parameters to a valid function signature.");
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

int lua_VRSensor_getType(lua_State* state)
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
                VRSensor* instance = getInstance(state);
                VRDevice::VRTypes result = instance->getType();

                // Push the return value onto the stack.
                lua_pushstring(state, lua_stringFromEnum_VRDeviceVRTypes(result));

                return 1;
            }

            lua_pushstring(state, "lua_VRSensor_getType - Failed to match the given parameters to a valid function signature.");
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

int lua_VRSensor_getVersion(lua_State* state)
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
                VRSensor* instance = getInstance(state);
                unsigned int result = instance->getVersion();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_VRSensor_getVersion - Failed to match the given parameters to a valid function signature.");
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

int lua_VRSensor_static_isVRSupported(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            bool result = VRSensor::isVRSupported();

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
