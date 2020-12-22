#ifndef _LUAU_UTIL_HPP
#define _LUAU_UTIL_HPP

#include <string>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

struct lua_State;

namespace Luau
{
namespace Util
{
static void insertData(lua_State* lua, const std::string& name, void* value)
{
    lua_pushstring(lua, name.c_str());
    lua_pushlightuserdata(lua, value);
    lua_settable(lua, -3);
}

static void insertBoolean(lua_State* lua, const std::string& name, bool value)
{
    lua_pushstring(lua, name.c_str());
    lua_pushboolean(lua, value);
    lua_settable(lua, -3);
}

static void insertString(lua_State* lua, const std::string& name, const std::string& value)
{
    lua_pushstring(lua, name.c_str());
    lua_pushstring(lua, value.c_str());
    lua_settable(lua, -3);
}

static void insertInteger(lua_State* lua, const std::string& name, int value)
{
    lua_pushstring(lua, name.c_str());
    lua_pushinteger(lua, value);
    lua_settable(lua, -3);
}

static void insertNumber(lua_State* lua, const std::string& name, float value)
{
    lua_pushstring(lua, name.c_str());
    lua_pushnumber(lua, static_cast<lua_Number>(value));
    lua_settable(lua, -3);
}

static void insertFunction(lua_State* lua, const std::string& name, int(*proc)(lua_State*))
{
    lua_pushstring(lua, name.c_str());
    lua_pushcfunction(lua, proc);
    lua_settable(lua, -3);
}
}
}

#endif 
