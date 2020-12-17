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

    static int width(lua_State*);
    static int height(lua_State*);
    static int background_color(lua_State*);
    static int load_font(lua_State*);

    static int add_event_listener(lua_State*);

private:
    static Canvas::Canvas* s_canvas;
};
}

#endif
