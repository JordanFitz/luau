#include "Luau.hpp"

int main(int argc, char** argv)
{
    int result = 0;
    bool reset = false;
    
    do
    {
        reset = false;

        Luau::Luau* luau = new Luau::Luau();

        luau->canvas()->addEventListener("keydown", [&](const Canvas::Event& e) {
            if (Canvas::eventAs<Canvas::KeyboardEvent>(e).code() == "F5")
            {
                luau->canvas()->close();
                reset = true;
                printf("!!! Reloading...\n");
            }
        });

        if (argc == 1)
        {
            result = luau->run("test.lua");
        }
        else
        {
            result = luau->run(argv[1]);
        }

        delete luau;
    } while (reset);    

    return result;
}
