#ifndef LUA_HMDRENDERMODE_H_
#define LUA_HMDRENDERMODE_H_

#include "HMD.h"

namespace gameplay
{

// Lua bindings for enum conversion functions for HMD::RenderMode.
HMD::RenderMode lua_enumFromString_HMDRenderMode(const char* s);
const char* lua_stringFromEnum_HMDRenderMode(HMD::RenderMode e);

}

#endif
