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

    static int clear_rect(lua_State*);

private:
    static Canvas::Context* s_context;
};
}

#endif
