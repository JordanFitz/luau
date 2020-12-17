#include "Luau.hpp"

int main(char** argv, int argc)
{
    Luau::Luau luau;
    return luau.run("test.lua");
}