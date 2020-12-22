#ifndef _LUAU_CONTEXT_HPP
#define _LUAU_CONTEXT_HPP

#include "Canvas.hpp"

struct lua_State;

namespace Luau
{
class LuaContext
{
public:
    static void initialize(Canvas::Context*);

    static int fill_style(lua_State*);
    static int stroke_style(lua_State*);

    static int line_width(lua_State*);
    static int line_join(lua_State*);
    static int line_cap(lua_State*);

    static int clear_rect(lua_State*);
    static int fill_rect(lua_State*);
    static int stroke_rect(lua_State*);

    static int draw_image(lua_State*);

    static int font(lua_State*);

    static int fill_text(lua_State*);
    static int stroke_text(lua_State*);

    static int begin_path(lua_State*);
    static int move_to(lua_State*);
    static int line_to(lua_State*);
    static int rect(lua_State*);
    static int stroke(lua_State*);
    static int fill(lua_State*);
    static int close_path(lua_State*);
    static int arc(lua_State*);

    static int measure_text(lua_State*);
    static int create_linear_gradient(lua_State*);

    static int add_color_stop(lua_State*);
    static int gradient_delete(lua_State*);

private:
    static Canvas::Context* s_context;
    static std::vector<Canvas::CanvasGradient> s_gradients;
};
}

#endif
