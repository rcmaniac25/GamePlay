#include "Base.h"
#include "lua_HMDRenderMode.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_HMDRenderMode_MONO_MODE = "MONO_MODE";
static const char* luaEnumString_HMDRenderMode_STEREO_MODE = "STEREO_MODE";
static const char* luaEnumString_HMDRenderMode_LEFT_RIGHT_MODE = "LEFT_RIGHT_MODE";

HMD::RenderMode lua_enumFromString_HMDRenderMode(const char* s)
{
    if (strcmp(s, luaEnumString_HMDRenderMode_MONO_MODE) == 0)
        return HMD::MONO_MODE;
    if (strcmp(s, luaEnumString_HMDRenderMode_STEREO_MODE) == 0)
        return HMD::STEREO_MODE;
    if (strcmp(s, luaEnumString_HMDRenderMode_LEFT_RIGHT_MODE) == 0)
        return HMD::LEFT_RIGHT_MODE;
    return HMD::MONO_MODE;
}

const char* lua_stringFromEnum_HMDRenderMode(HMD::RenderMode e)
{
    if (e == HMD::MONO_MODE)
        return luaEnumString_HMDRenderMode_MONO_MODE;
    if (e == HMD::STEREO_MODE)
        return luaEnumString_HMDRenderMode_STEREO_MODE;
    if (e == HMD::LEFT_RIGHT_MODE)
        return luaEnumString_HMDRenderMode_LEFT_RIGHT_MODE;
    return enumStringEmpty;
}

}

