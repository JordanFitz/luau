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

    bool _checkUserDefined(const std::string&);
    bool _callUserDefined(const std::string&);

    bool _luaRender();
    bool _luaUpdate();

    void _addFunctionToTable(const std::string&, int(*proc)(lua_State*));
};
}

#endif
