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

    auto eventHandler = [&](const Canvas::Event& e) { _handleEvent(e); };

    m_canvas.addEventListener("unload", eventHandler);
    m_canvas.addEventListener("resize", eventHandler);
    m_canvas.addEventListener("keydown", eventHandler);
    m_canvas.addEventListener("keyup", eventHandler);
    m_canvas.addEventListener("mousedown", eventHandler);
    m_canvas.addEventListener("mouseup", eventHandler);
    m_canvas.addEventListener("mousemove", eventHandler);
    m_canvas.addEventListener("wheel", eventHandler);
    m_canvas.addEventListener("focus", eventHandler);
    m_canvas.addEventListener("blur", eventHandler);
}

Luau::~Luau()
{
    lua_close(m_lua);
}

int Luau::run(const std::string& scriptPath)
{
    if (!_checkResult(m_lua, luaL_dofile(m_lua, scriptPath.c_str())))
        RETURN_CANVAS_INIT;

    if (!_checkGlobal("render") || !_checkGlobal("update"))
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

bool Luau::_checkGlobal(const std::string& name, bool printError)
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
        if(printError)
            printf("Failed to find function 'canvas.%s'\n", name.c_str());
        return false;
    }

    return true;
}

bool Luau::_callGlobal(const std::string& name)
{
    if (!_checkGlobal(name))
        return false;

    // Call the function with no arguments and no return value
    if (!_checkResult(m_lua, lua_pcall(m_lua, 0, 0, 0)))
        return false;

    // Seems like you have to pop the return value off, even
    // when there isn't one
    lua_pop(m_lua, 1);

    return true;
}

void Luau::_handleEvent(const Canvas::Event& e)
{
    const std::string name = "on" + e.name();

    if (!_checkGlobal(name, false))
        return;

    _eventToTable(e);

    if (!_checkResult(m_lua, lua_pcall(m_lua, 1, 0, 0)))
        return;

    lua_pop(m_lua, 1);
}

bool Luau::_luaRender()
{
    return _callGlobal("render");
}

bool Luau::_luaUpdate()
{
    return _callGlobal("update");
}

void Luau::_eventToTable(const Canvas::Event& e)
{
    lua_newtable(m_lua);

    switch (e.type())
    {
    case Canvas::EventType::Keyboard:
    {
        auto& event = Canvas::eventAs<Canvas::KeyboardEvent>(e);

        _insertBoolean("altKey", event.altKey());
        _insertBoolean("ctrlKey", event.ctrlKey());
        _insertBoolean("shiftKey", event.shiftKey());
        _insertBoolean("metaKey", event.metaKey());

        // TODO: getModifierState

        _insertString("key", event.key());
        _insertString("code", event.code());

        break;
    }

    case Canvas::EventType::Mouse:
    {
        auto& event = Canvas::eventAs<Canvas::MouseEvent>(e);

        _insertInteger("button", event.button());
        _insertInteger("buttons", event.buttons());

        _insertInteger("clientX", event.clientX());
        _insertInteger("clientY", event.clientY());
        _insertInteger("screenX", event.screenX());
        _insertInteger("screenY", event.screenY());

        _insertBoolean("altKey", event.altKey());
        _insertBoolean("ctrlKey", event.ctrlKey());
        _insertBoolean("shiftKey", event.shiftKey());
        _insertBoolean("metaKey", event.metaKey());

        break;
    }

    case Canvas::EventType::Wheel:
    {
        auto& event = Canvas::eventAs<Canvas::WheelEvent>(e);

        _insertNumber("deltaX", event.deltaX());
        _insertNumber("deltaY", event.deltaY());

        break;
    }

    default: break;
    }
}

void Luau::_insertBoolean(const std::string& name, bool value)
{
    lua_pushstring(m_lua, name.c_str());
    lua_pushboolean(m_lua, value);
    lua_settable(m_lua, -3);
}

void Luau::_insertString(const std::string& name, const std::string& value)
{
    lua_pushstring(m_lua, name.c_str());
    lua_pushstring(m_lua, value.c_str());
    lua_settable(m_lua, -3);
}

void Luau::_insertInteger(const std::string& name, int value)
{
    lua_pushstring(m_lua, name.c_str());
    lua_pushinteger(m_lua, value);
    lua_settable(m_lua, -3);
}

void Luau::_insertNumber(const std::string& name, float value)
{
    lua_pushstring(m_lua, name.c_str());
    lua_pushnumber(m_lua, static_cast<lua_Number>(value));
    lua_settable(m_lua, -3);
}

}