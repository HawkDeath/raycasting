#include "RaycasterDemo.h"
#include "Log.h"

#include <memory>

int main(int argc, char *argv[])
{
    UNUSE(argc);
    UNUSE(argv);

    try
    {
        auto ray_app = std::make_unique<ray::RaycasterDemo>();
        ray_app->run();
    }
    catch (std::exception &e)
    {
        ELOG("[exception] {}", e.what());
        return -1;
    }

    return 0;
}