#include <memory>

#include "Window.h"

int main() {

    auto window = std::make_unique<ray::Window>("raycasting", 1920, 1080);

    while (window->is_running())
    {
        glfwPollEvents();
    }

    return 0;
}