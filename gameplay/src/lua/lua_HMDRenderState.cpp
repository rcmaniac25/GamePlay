#include "Base.h"
#include "lua_HMDRenderState.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_HMDRenderState_LEFT_EYE_RENDERING = "LEFT_EYE_RENDERING";
static const char* luaEnumString_HMDRenderState_RIGHT_EYE_RENDERING = "RIGHT_EYE_RENDERING";
static const char* luaEnumString_HMDRenderState_MONO_RENDERING = "MONO_RENDERING";
static const char* luaEnumString_HMDRenderState_NOT_RENDERING = "NOT_RENDERING";

HMD::RenderState lua_enumFromString_HMDRenderState(const char* s)
{
    if (strcmp(s, luaEnumString_HMDRenderState_LEFT_EYE_RENDERING) == 0)
        return HMD::LEFT_EYE_RENDERING;
    if (strcmp(s, luaEnumString_HMDRenderState_RIGHT_EYE_RENDERING) == 0)
        return HMD::RIGHT_EYE_RENDERING;
    if (strcmp(s, luaEnumString_HMDRenderState_MONO_RENDERING) == 0)
        return HMD::MONO_RENDERING;
    if (strcmp(s, luaEnumString_HMDRenderState_NOT_RENDERING) == 0)
        return HMD::NOT_RENDERING;
    return HMD::LEFT_EYE_RENDERING;
}

const char* lua_stringFromEnum_HMDRenderState(HMD::RenderState e)
{
    if (e == HMD::LEFT_EYE_RENDERING)
        return luaEnumString_HMDRenderState_LEFT_EYE_RENDERING;
    if (e == HMD::RIGHT_EYE_RENDERING)
        return luaEnumString_HMDRenderState_RIGHT_EYE_RENDERING;
    if (e == HMD::MONO_RENDERING)
        return luaEnumString_HMDRenderState_MONO_RENDERING;
    if (e == HMD::NOT_RENDERING)
        return luaEnumString_HMDRenderState_NOT_RENDERING;
    return enumStringEmpty;
}

}

