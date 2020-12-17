#ifndef _LUAU_HPP
#define _LUAU_HPP

#include <string>

#include "Canvas.hpp"

struct lua_State;

namespace Luau
{
class LuaCanvas;

class Luau
{
public:
    Luau();
    ~Luau();

    int run(const std::string&);

private:
    lua_State* m_lua;
    Canvas::Canvas m_canvas;

    bool _checkResult(lua_State*, int);
    bool _getGlobalTable(const std::string&);
    bool _getCanvasTable();
    bool _getContextTable();

    bool _checkGlobal(const std::string&, bool p = true);
    bool _callGlobal(const std::string&);

    void _handleEvent(const Canvas::Event&);
    void _eventToTable(const Canvas::Event&);

    void _insertBoolean(const std::string&, bool);
    void _insertInteger(const std::string&, int);
    void _insertNumber(const std::string&, float);
    void _insertString(const std::string&, const std::string&);
    void _insertData(const std::string&, void*);

    bool _luaRender();
    bool _luaUpdate(float);

    void _addFunctionToTable(const std::string&, int(*proc)(lua_State*));

    // For use in Lua
    static int get_modifier_state(lua_State*);
};
}

#endif
