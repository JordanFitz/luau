extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "LuaCanvas.hpp"

#define CHECK_INITIALIZED()                              \
if (s_canvas == nullptr)                                 \
{                                                        \
    printf("The LuaCanvas has not been initialized!\n"); \
    return 0;                                            \
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
Canvas::Canvas* LuaCanvas::s_canvas = nullptr;

void LuaCanvas::initialize(Canvas::Canvas* canvas)
{
    s_canvas = canvas;
}

int LuaCanvas::set_width(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushinteger(lua, s_canvas->width());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto width = static_cast<unsigned int>(luaL_checkinteger(lua, 1));
    s_canvas->width(width);

    return 0;
}

int LuaCanvas::set_height(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushinteger(lua, s_canvas->height());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto height = static_cast<unsigned int>(luaL_checkinteger(lua, 1));
    s_canvas->height(height);

    return 0;
}

int LuaCanvas::set_background_color(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushstring(lua, s_canvas->backgroundColor().c_str());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto color = luaL_checkstring(lua, 1);
    s_canvas->backgroundColor(color);

    return 0;
}
}
