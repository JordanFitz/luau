extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>

#include "Canvas.hpp"
#include "Image.hpp"
#include "Audio.hpp"

#include "LuaCanvas.hpp"
#include "LuaContext.hpp"

#include "Util.hpp"

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
        _addFunctionToTable("max_framerate", LuaCanvas::max_framerate);
        _addFunctionToTable("use_vsync", LuaCanvas::use_vsync);
        _addFunctionToTable("is_key_pressed", LuaCanvas::is_key_pressed);
        _addFunctionToTable("title", LuaCanvas::title);

        // TODO: Maybe this should be a context method rather than canvas?
        _addFunctionToTable("view_position", LuaCanvas::view_position);
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
        _addFunctionToTable("draw_image", LuaContext::draw_image);
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
        _addFunctionToTable("measure_text", LuaContext::measure_text);
        _addFunctionToTable("create_linear_gradient", LuaContext::create_linear_gradient);
    }
    lua_setglobal(m_lua, "context");

    // Image table
    lua_newtable(m_lua);
    {
        _addFunctionToTable("new", Static::image_new);
    }
    lua_setglobal(m_lua, "Image");

    // Audio table
    lua_newtable(m_lua);
    {
        _addFunctionToTable("new", Static::audio_new);
    }
    lua_setglobal(m_lua, "Audio");

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

Canvas::Canvas* Luau::canvas()
{
    return &m_canvas;
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

    m_canvas.hookUpdate([&](Canvas::Canvas& canvas, float delta) {
        _luaUpdate(delta);
    });

    m_canvas.hookRender([&](Canvas::Canvas& canvas) {
        _luaRender();
    });

    RETURN_CANVAS_INIT;
}

void Luau::_addFunctionToTable(const std::string& name, int(*proc)(lua_State*))
{
    Util::insertFunction(m_lua, name, proc);
}

bool Luau::_checkResult(lua_State* lua, int result)
{
    if (result != LUA_OK)
    {
        printf("%s\n", lua_tostring(lua, -1));
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

bool Luau::_luaUpdate(float delta)
{
    if (!_checkGlobal("update"))
        return false;

    lua_pushnumber(m_lua, static_cast<lua_Number>(delta));
    
    if (!_checkResult(m_lua, lua_pcall(m_lua, 1, 0, 0)))
        return false;

    lua_pop(m_lua, 1);

    return true;
}

void Luau::_eventToTable(const Canvas::Event& e)
{
    lua_newtable(m_lua);

    Util::insertData(m_lua, "ptr", (void*)(&e));

    switch (e.type())
    {
    case Canvas::EventType::Keyboard:
    {
        auto& event = Canvas::eventAs<Canvas::KeyboardEvent>(e);

        Util::insertBoolean(m_lua, "altKey", event.altKey());
        Util::insertBoolean(m_lua, "ctrlKey", event.ctrlKey());
        Util::insertBoolean(m_lua, "shiftKey", event.shiftKey());
        Util::insertBoolean(m_lua, "metaKey", event.metaKey());

        // TODO: getModifierState

        Util::insertString(m_lua, "key", event.key());
        Util::insertString(m_lua, "code", event.code());

        _addFunctionToTable("get_modifier_state", Static::get_modifier_state);

        break;
    }

    case Canvas::EventType::Mouse:
    {
        auto& event = Canvas::eventAs<Canvas::MouseEvent>(e);

        Util::insertInteger(m_lua, "button", event.button());
        Util::insertInteger(m_lua, "buttons", event.buttons());

        Util::insertInteger(m_lua, "clientX", event.clientX());
        Util::insertInteger(m_lua, "clientY", event.clientY());
        Util::insertInteger(m_lua, "screenX", event.screenX());
        Util::insertInteger(m_lua, "screenY", event.screenY());

        Util::insertBoolean(m_lua, "altKey", event.altKey());
        Util::insertBoolean(m_lua, "ctrlKey", event.ctrlKey());
        Util::insertBoolean(m_lua, "shiftKey", event.shiftKey());
        Util::insertBoolean(m_lua, "metaKey", event.metaKey());

        break;
    }

    case Canvas::EventType::Wheel:
    {
        auto& event = Canvas::eventAs<Canvas::WheelEvent>(e);

        Util::insertNumber(m_lua, "deltaX", event.deltaX());
        Util::insertNumber(m_lua, "deltaY", event.deltaY());

        break;
    }

    default: break;
    }
}



// Static methods for use in Lua

int Luau::Static::image_new(lua_State* lua)
{
    if (lua_gettop(lua) != 1 && lua_gettop(lua) != 2)
    {
        printf("Wrong number of arguments to 'Image:new()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    const char* src = nullptr;

    if (lua_gettop(lua) == 2)
    {
        src = luaL_checkstring(lua, 2);
    }

    Canvas::Image* image;

    // I think it should be fine that these are never `delete`d because
    // they can exist for the lifetime of the program?

    if (src != nullptr)
    {
        image = new Canvas::Image(src);
    }
    else
    {
        image = new Canvas::Image();
    }

    lua_newtable(lua);
    {
        Util::insertData(lua, "ptr", image);
        Util::insertFunction(lua, "src", image_src);

        lua_newtable(lua);
        Util::insertFunction(lua, "__gc", image_delete);
        lua_setmetatable(lua, -2);
    }

    return 1;
}


int Luau::Static::image_delete(lua_State* lua)
{
    if (lua_gettop(lua) != 1)
    {
        printf("Wrong number of arguments to 'image:delete()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    auto image = static_cast<Canvas::Image*>(ptr);

    delete image;

    return 0;
}

int Luau::Static::image_src(lua_State* lua)
{
    if (lua_gettop(lua) != 1 && lua_gettop(lua) != 2)
    {
        printf("Wrong number of arguments to 'image:src()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    const char* src = nullptr;

    if (lua_gettop(lua) == 2)
    {
        src = luaL_checkstring(lua, 2);
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    auto image = static_cast<Canvas::Image*>(ptr);

    if (src != nullptr)
    {
        image->src(src);
        return 0;
    }

    lua_pushstring(lua, image->src().c_str());

    return 1;
}


int Luau::Static::audio_new(lua_State* lua)
{
    if (lua_gettop(lua) != 1 && lua_gettop(lua) != 2 && lua_gettop(lua) != 3)
    {
        printf("Wrong number of arguments to 'Audio:new()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    const char* src = nullptr;
    bool stream = false;

    if (lua_gettop(lua) > 1)
    {
        src = luaL_checkstring(lua, 2);
    }

    if (lua_gettop(lua) == 3)
    {
        stream = lua_toboolean(lua, 3);
    }

    Canvas::Audio* audio;

    if (src != nullptr)
    {
        audio = new Canvas::Audio(src, stream);
    }
    else
    {
        audio = new Canvas::Audio();
    }

    lua_newtable(lua);
    {
        Util::insertData(lua, "ptr", audio);
        Util::insertFunction(lua, "src", audio_src);
        Util::insertFunction(lua, "play", audio_play);
        Util::insertFunction(lua, "pause", audio_pause);
        Util::insertFunction(lua, "stop", audio_stop);
        Util::insertFunction(lua, "playing", audio_playing);

        lua_newtable(lua);
        Util::insertFunction(lua, "__gc", audio_delete);
        lua_setmetatable(lua, -2);
    }

    return 1;
}

int Luau::Static::audio_delete(lua_State* lua)
{
    if (lua_gettop(lua) != 1)
    {
        printf("Wrong number of arguments to 'audio:delete()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    auto audio = static_cast<Canvas::Audio*>(ptr);

    delete audio;

    return 0;
}

int Luau::Static::audio_src(lua_State* lua)
{
    if (lua_gettop(lua) != 1 && lua_gettop(lua) != 2)
    {
        printf("Wrong number of arguments to 'audio:src()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    const char* src = nullptr;

    if (lua_gettop(lua) == 2)
    {
        src = luaL_checkstring(lua, 2);
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    auto audio = static_cast<Canvas::Audio*>(ptr);

    if (src != nullptr)
    {
        audio->src(src);
        return 0;
    }

    lua_pushstring(lua, audio->src().c_str());

    return 1;
}

int Luau::Static::audio_play(lua_State* lua)
{
    if (lua_gettop(lua) != 1)
    {
        printf("Wrong number of arguments to 'audio:play()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    static_cast<Canvas::Audio*>(ptr)->play();

    return 0;
}

int Luau::Static::audio_pause(lua_State* lua)
{
    if (lua_gettop(lua) != 1)
    {
        printf("Wrong number of arguments to 'audio:pause()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    static_cast<Canvas::Audio*>(ptr)->pause();

    return 0;
}

int Luau::Static::audio_stop(lua_State* lua)
{
    if (lua_gettop(lua) != 1)
    {
        printf("Wrong number of arguments to 'audio:stop()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    static_cast<Canvas::Audio*>(ptr)->stop();

    return 0;
}

int Luau::Static::audio_playing(lua_State* lua)
{
    if (lua_gettop(lua) != 1)
    {
        printf("Wrong number of arguments to 'audio:playing()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);
    bool playing = static_cast<Canvas::Audio*>(ptr)->playing();

    lua_pushboolean(lua, playing);

    return 1;
}


int Luau::Static::get_modifier_state(lua_State* lua)
{
    if (lua_gettop(lua) != 2)
    {
        printf("Wrong number of arguments to 'get_modifier_state()'\n");
        return 0;
    }

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    auto key = luaL_checkstring(lua, 2);

    lua_getfield(lua, 1, "ptr");

    auto ptr = lua_touserdata(lua, -1);

    auto event = static_cast<Canvas::KeyboardEvent*>(ptr);

    lua_pushboolean(lua, event->getModifierState(key));

    return 1;
}
}