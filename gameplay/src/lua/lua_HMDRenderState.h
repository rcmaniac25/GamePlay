#ifndef LUA_HMDRENDERSTATE_H_
#define LUA_HMDRENDERSTATE_H_

#include "HMD.h"

namespace gameplay
{

// Lua bindings for enum conversion functions for HMD::RenderState.
HMD::RenderState lua_enumFromString_HMDRenderState(const char* s);
const char* lua_stringFromEnum_HMDRenderState(HMD::RenderState e);

}

#endif
