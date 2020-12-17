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
//       Not sure if __func__ is appropriate in all
//       cases?
#define CHECK_ARG_COUNT(count)                                 \
if (lua_gettop(lua) != count)                                  \
{                                                              \
    printf("Wrong number of arguments to '%s()'\n", __func__); \
    return 0;                                                  \
}

namespace Luau
{
Canvas::Canvas* LuaCanvas::s_canvas = nullptr;

void LuaCanvas::initialize(Canvas::Canvas* canvas)
{
    s_canvas = canvas;
}

int LuaCanvas::width(lua_State* lua)
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

int LuaCanvas::height(lua_State* lua)
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

int LuaCanvas::background_color(lua_State* lua)
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

int LuaCanvas::load_font(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(2);

    auto name = luaL_checkstring(lua, 1);
    auto path = luaL_checkstring(lua, 2);

    s_canvas->loadFont(name, path);

    return 0;
}

int LuaCanvas::add_event_listener(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(-1);

    auto name = luaL_checkstring(lua, 1);
    
    // There is no luaL_checkfunction. I think this does
    // basically the same thing as checkfunction would.
    if (!lua_isfunction(lua, 2))
    {
        luaL_typeerror(lua, 2, "function");
        return 0;
    }

    return 0;
}

int LuaCanvas::max_framerate(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushinteger(lua, s_canvas->maxFramerate());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto framerate = static_cast<unsigned int>(luaL_checkinteger(lua, 1));

    s_canvas->maxFramerate(framerate);

    return 0;
}

int LuaCanvas::use_vsync(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushboolean(lua, s_canvas->vsync());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    bool vsync = lua_toboolean(lua, 1);

    s_canvas->vsync(vsync);

    return 0;
}

}
