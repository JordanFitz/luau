extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "TextMetrics.hpp"

#include "Util.hpp"
#include "LuaContext.hpp"

#define CHECK_INITIALIZED()                               \
if (s_context == nullptr)                                 \
{                                                         \
    printf("The LuaContext has not been initialized!\n"); \
    return 0;                                             \
}

// TODO: Make this error message more informative.
#define CHECK_ARG_COUNT(count)                                 \
if (lua_gettop(lua) != count)                                  \
{                                                              \
    printf("Wrong number of arguments to '%s()'\n", __func__); \
    return 0;                                                  \
}

namespace Luau
{
Canvas::Context* LuaContext::s_context = nullptr;
std::vector<Canvas::CanvasGradient> LuaContext::s_gradients;

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

int LuaContext::fill_style(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushstring(lua, s_context->fillStyle().c_str());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    if (lua_isstring(lua, 1))
    {
        auto style = luaL_checkstring(lua, 1);
        s_context->fillStyle(style);
    }
    else if (lua_istable(lua, 1))
    {
        lua_getfield(lua, 1, "ptr");
        auto ptr = lua_touserdata(lua, -1);
        auto gradient = static_cast<Canvas::CanvasGradient*>(ptr);
        s_context->fillStyle(*gradient);
    }

    return 0;
}

int LuaContext::stroke_style(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushstring(lua, s_context->strokeStyle().c_str());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto style = luaL_checkstring(lua, 1);
    s_context->strokeStyle(style);

    return 0;
}

int LuaContext::line_width(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushnumber(lua, static_cast<lua_Number>(s_context->lineWidth()));
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto width = static_cast<float>(luaL_checknumber(lua, 1));
    s_context->lineWidth(width);

    return 0;
}

int LuaContext::line_join(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushstring(lua, s_context->lineJoin().c_str());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto join = luaL_checkstring(lua, 1);
    s_context->lineJoin(join);

    return 0;
}

int LuaContext::line_cap(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushstring(lua, s_context->lineCap().c_str());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto cap = luaL_checkstring(lua, 1);
    s_context->lineCap(cap);

    return 0;
}

int LuaContext::fill_rect(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(4);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));
    float w = static_cast<float>(luaL_checknumber(lua, 3));
    float h = static_cast<float>(luaL_checknumber(lua, 4));

    s_context->fillRect(x, y, w, h);

    return 0;
}

int LuaContext::stroke_rect(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(4);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));
    float w = static_cast<float>(luaL_checknumber(lua, 3));
    float h = static_cast<float>(luaL_checknumber(lua, 4));

    s_context->strokeRect(x, y, w, h);

    return 0;
}

int LuaContext::draw_image(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 3 || lua_gettop(lua) == 5|| lua_gettop(lua) == 9)
    {
        if (!lua_istable(lua, 1))
        {
            luaL_typeerror(lua, 1, "table");
            return 0;
        }

        if (lua_gettop(lua) == 3)
        {
            float dx = static_cast<float>(luaL_checknumber(lua, 2));
            float dy = static_cast<float>(luaL_checknumber(lua, 3));

            lua_getfield(lua, 1, "ptr");
            auto ptr = lua_touserdata(lua, -1);
            auto image = static_cast<Canvas::Image*>(ptr);

            s_context->drawImage(*image, dx, dy);

            return 0;
        }

        if (lua_gettop(lua) == 5)
        {
            float dx = static_cast<float>(luaL_checknumber(lua, 2));
            float dy = static_cast<float>(luaL_checknumber(lua, 3));
            float dWidth = static_cast<float>(luaL_checknumber(lua, 4));
            float dHeight = static_cast<float>(luaL_checknumber(lua, 5));

            lua_getfield(lua, 1, "ptr");
            auto ptr = lua_touserdata(lua, -1);
            auto image = static_cast<Canvas::Image*>(ptr);

            s_context->drawImage(*image, dx, dy, dWidth, dHeight);

            return 0;
        }

        if (lua_gettop(lua) == 9)
        {
            float sx = static_cast<float>(luaL_checknumber(lua, 2));
            float sy = static_cast<float>(luaL_checknumber(lua, 3));
            float sWidth = static_cast<float>(luaL_checknumber(lua, 4));
            float sHeight = static_cast<float>(luaL_checknumber(lua, 5));

            float dx = static_cast<float>(luaL_checknumber(lua, 6));
            float dy = static_cast<float>(luaL_checknumber(lua, 7));
            float dWidth = static_cast<float>(luaL_checknumber(lua, 8));
            float dHeight = static_cast<float>(luaL_checknumber(lua, 9));

            lua_getfield(lua, 1, "ptr");
            auto ptr = lua_touserdata(lua, -1);
            auto image = static_cast<Canvas::Image*>(ptr);

            s_context->drawImage(*image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight);

            return 0;
        }
    }

    CHECK_ARG_COUNT(-1);

    return 0;
}

int LuaContext::font(lua_State* lua)
{
    CHECK_INITIALIZED();

    if (lua_gettop(lua) == 0)
    {
        lua_pushstring(lua, s_context->font().c_str());
        return 1;
    }

    CHECK_ARG_COUNT(1);

    auto font = luaL_checkstring(lua, 1);
    s_context->font(font);

    return 0;
}

int LuaContext::fill_text(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(3);

    auto string = luaL_checkstring(lua, 1);
    float x = static_cast<float>(luaL_checknumber(lua, 2));
    float y = static_cast<float>(luaL_checknumber(lua, 3));

    s_context->fillText(string, x, y);

    return 0;
}

int LuaContext::stroke_text(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(3);

    auto string = luaL_checkstring(lua, 1);
    float x = static_cast<float>(luaL_checknumber(lua, 2));
    float y = static_cast<float>(luaL_checknumber(lua, 3));

    s_context->strokeText(string, x, y);

    return 0;
}

int LuaContext::begin_path(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(0);

    s_context->beginPath();

    return 0;
}

int LuaContext::move_to(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(2);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));

    s_context->moveTo(x, y);

    return 0;
}

int LuaContext::line_to(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(2);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));

    s_context->lineTo(x, y);

    return 0;
}

int LuaContext::rect(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(4);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));
    float w = static_cast<float>(luaL_checknumber(lua, 3));
    float h = static_cast<float>(luaL_checknumber(lua, 4));

    s_context->rect(x, y, w, h);

    return 0;
}

int LuaContext::stroke(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(0);

    s_context->stroke();

    return 0;
}

int LuaContext::fill(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(0);

    s_context->fill();

    return 0;
}

int LuaContext::close_path(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(0);

    s_context->closePath();

    return 0;
}

int LuaContext::arc(lua_State* lua)
{
    CHECK_INITIALIZED();
    
    bool hasAnticlockwise = false;

    if (lua_gettop(lua) == 6)
        hasAnticlockwise = true;
    else if (lua_gettop(lua) == 5)
        hasAnticlockwise = false; // Redundant, but makes things more obvious
    else CHECK_ARG_COUNT(-1);

    float x = static_cast<float>(luaL_checknumber(lua, 1));
    float y = static_cast<float>(luaL_checknumber(lua, 2));
    float radius = static_cast<float>(luaL_checknumber(lua, 3));
    float startAngle = static_cast<float>(luaL_checknumber(lua, 4));
    float endAngle = static_cast<float>(luaL_checknumber(lua, 5));

    bool anticlockwise = false;

    if (hasAnticlockwise)
        anticlockwise = lua_toboolean(lua, 6);

    s_context->arc(x, y, radius, startAngle, endAngle, anticlockwise);

    return 0;
}

int LuaContext::measure_text(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(1);

    auto text = luaL_checkstring(lua, 1);
    auto metric = s_context->measureText(text);

    lua_newtable(lua);
    {
        Util::insertNumber(lua, "width", metric.width);
    }

    return 1;
}

int LuaContext::create_linear_gradient(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(4);

    float x1 = static_cast<float>(luaL_checknumber(lua, 1));
    float y1 = static_cast<float>(luaL_checknumber(lua, 2));
    float x2 = static_cast<float>(luaL_checknumber(lua, 3));
    float y2 = static_cast<float>(luaL_checknumber(lua, 4));

    auto gradient = new Canvas::CanvasGradient(s_context, x1, y1, x2, y2);

    lua_newtable(lua);
    {
        Util::insertData(lua, "ptr", gradient);
        Util::insertFunction(lua, "add_color_stop", add_color_stop);

        lua_newtable(lua);
        {
            Util::insertFunction(lua, "__gc", gradient_delete);
            lua_setmetatable(lua, -2);
        }
    }

    return 1;
}

int LuaContext::gradient_delete(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(1);

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    lua_getfield(lua, 1, "ptr");
    auto ptr = lua_touserdata(lua, -1);
    auto gradient = static_cast<Canvas::CanvasGradient*>(ptr);

    printf("delete %p\n", gradient);

    delete gradient;

    return 0;
}

int LuaContext::add_color_stop(lua_State* lua)
{
    CHECK_INITIALIZED();
    CHECK_ARG_COUNT(3);

    if (!lua_istable(lua, 1))
    {
        luaL_typeerror(lua, 1, "table");
        return 0;
    }

    float offset = static_cast<float>(luaL_checknumber(lua, 2));
    auto color = luaL_checkstring(lua, 3);

    lua_getfield(lua, 1, "ptr");
    auto ptr = lua_touserdata(lua, -1);
    auto gradient = static_cast<Canvas::CanvasGradient*>(ptr);

    printf("%.2f, %s\n", offset, color);
    gradient->addColorStop(offset, color);

    return 0;
}
}
