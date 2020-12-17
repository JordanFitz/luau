extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>

#include "Canvas.hpp"

#include "LuaCanvas.hpp"
#include "LuaContext.hpp"

#include "Luau.hpp"

#define RETURN_CANVAS_INIT return m_canvas.initialize()

namespace Luau
{
Luau::Luau()
{
    LuaCanvas::initialize(&m_canvas);
    LuaContext::initialize(&m_canvas.getContext());

    m_lua = luaL_newstate();
    luaL_openlibs(m_lua);
    
    // 'canvas' table
    lua_newtable(m_lua);
    {
        _addFunctionToTable("width", LuaCanvas::width);
        _addFunctionToTable("height", LuaCanvas::height);
        _addFunctionToTable("background_color", LuaCanvas::background_color);
        _addFunctionToTable("load_font", LuaCanvas::load_font);
    }
    lua_setglobal(m_lua, "canvas");

    // 'context' table
    lua_newtable(m_lua);
    {
        _addFunctionToTable("fill_style", LuaContext::fill_style);
        _addFunctionToTable("stroke_style", LuaContext::stroke_style);
        _addFunctionToTable("line_width", LuaContext::line_width);
        _addFunctionToTable("line_join", LuaContext::line_join);
        _addFunctionToTable("line_cap", LuaContext::line_cap);
        _addFunctionToTable("clear_rect", LuaContext::clear_rect);
        _addFunctionToTable("fill_rect", LuaContext::fill_rect);
        _addFunctionToTable("stroke_rect", LuaContext::stroke_rect);
        _addFunctionToTable("font", LuaContext::font);
        _addFunctionToTable("fill_text", LuaContext::fill_text);
        _addFunctionToTable("stroke_text", LuaContext::stroke_text);
        _addFunctionToTable("begin_path", LuaContext::begin_path);
        _addFunctionToTable("move_to", LuaContext::move_to);
        _addFunctionToTable("line_to", LuaContext::line_to);
        _addFunctionToTable("rect", LuaContext::rect);
        _addFunctionToTable("stroke", LuaContext::stroke);
        _addFunctionToTable("fill", LuaContext::fill);
        _addFunctionToTable("close_path", LuaContext::close_path);
        _addFunctionToTable("arc", LuaContext::arc);
    }
    lua_setglobal(m_lua, "context");
}

Luau::~Luau()
{
    lua_close(m_lua);
}

void Luau::_addFunctionToTable(const std::string& name, int(*proc)(lua_State*))
{
    lua_pushstring(m_lua, name.c_str());
    lua_pushcfunction(m_lua, proc);
    lua_settable(m_lua, -3);
}

bool Luau::_checkResult(lua_State* lua, int result)
{
    if (result != LUA_OK)
    {
        printf("Lua error: %s\n", lua_tostring(lua, -1));
        return false;
    }

    return true;
}

bool Luau::_getGlobalTable(const std::string& name)
{
    lua_getglobal(m_lua, name.c_str());

    if (!lua_istable(m_lua, -1))
        return false;

    return true;
}

bool Luau::_getCanvasTable()
{
    return _getGlobalTable("canvas");
}

bool Luau::_getContextTable()
{
    return _getGlobalTable("context");
}

bool Luau::_checkUserDefined(const std::string& name)
{
    if (!_getCanvasTable())
    {
        printf("Failed to get 'canvas' table\n");
        return false;
    }

    lua_pushstring(m_lua, name.c_str());
    lua_gettable(m_lua, -2);

    if (!lua_isfunction(m_lua, -1))
    {
        printf("Failed to find function 'canvas.%s'\n", name.c_str());
        return false;
    }

    return true;
}

bool Luau::_callUserDefined(const std::string& name)
{
    if (!_checkUserDefined(name))
        return false;

    // Call the function with no arguments and no return value
    if (!_checkResult(m_lua, lua_pcall(m_lua, 0, 0, 0)))
        return false;

    // Seems like you have to pop the return value of, even
    // when there isn't one
    lua_pop(m_lua, 1);

    return true;
}

bool Luau::_luaRender()
{
    return _callUserDefined("render");
}

bool Luau::_luaUpdate()
{
    return _callUserDefined("update");
}

int Luau::run(const std::string& scriptPath)
{
    if (!_checkResult(m_lua, luaL_dofile(m_lua, scriptPath.c_str())))
        RETURN_CANVAS_INIT;

    if(!_checkUserDefined("render") || !_checkUserDefined("update"))
    {
        printf("You must define both canvas.update and canvas.render functions\n");
        RETURN_CANVAS_INIT;
    }

    m_canvas.hookRender([&](Canvas::Canvas& canvas) {
        _luaRender();
    });

    m_canvas.hookUpdate([&](Canvas::Canvas& canvas) {
        _luaUpdate();
    });

    RETURN_CANVAS_INIT;
}
}