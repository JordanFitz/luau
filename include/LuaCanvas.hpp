#ifndef _LUAU_CANVAS_HPP
#define _LUAU_CANVAS_HPP

#include "Canvas.hpp"

struct lua_State;

namespace Luau
{
class LuaCanvas
{
public:
    static void initialize(Canvas::Canvas*);

    static int set_width(lua_State*);
    static int set_height(lua_State*);
    static int set_background_color(lua_State*);

    // TODO: addEventListener

private:
    static Canvas::Canvas* s_canvas;
};
}

#endif