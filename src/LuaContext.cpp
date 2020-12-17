extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "LuaContext.hpp"

#define CHECK_INITIALIZED()                               \
if (s_context == nullptr)                                 \
{                                                         \
    printf("The LuaContext has not been initialized!\n"); \
    return 0;                                             \
}

// TODO: Make this error message more informative.
#define CHECK_ARG_COUNT(count)              \
if (lua_gettop(lua) != count)               \
{                                           \
    printf("Wrong number of arguments!\n"); \
    return 0;                               \
}

namespace Luau
{
Canvas::Context* LuaContext::s_context = nullptr;

void LuaContext::initialize(Canvas::Context* context)
{
    s_context = context;
}

int LuaContext::clear_rect(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        s_context->clearRect();
        return 0;
    }

    CHECK_ARG_COUNT(4);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));
    float w = static_cast<float>(luaL_checknumber(lua, 3));
    float h = static_cast<float>(luaL_checknumber(lua, 4));

    s_context->clearRect(x, y, w, h);

    return 0;
}
}