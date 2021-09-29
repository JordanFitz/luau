#include "Luau.hpp"

int main(int argc, char** argv)
{
    Luau::Luau luau;

    if (argc == 1)
    {
        return luau.run("test.lua");
    }
    
    return luau.run(argv[1]);
}
