//
// Created by Patryk on 12/29/2023.
//
#include "Window.h"
#include "Log.h"

namespace ray
{
    Window::Window(const std::string &_title, const uint32_t &_width, const uint32_t &_height) : window_handler{nullptr}, width{_width}, height{_height}
    {
        glfwSetErrorCallback([](int code, const char *desc) -> void {ELOG("[GLFW_error] {}, {}]", code, desc);});
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_handler = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), _title.c_str(), nullptr, nullptr);
        if (window_handler == nullptr)
            RT_THROW("Failed to create window");

        glfwSetWindowUserPointer(window_handler, this);
        LOG("GLFW version {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    }

    Window::~Window()
    {
        if (window_handler != nullptr)
            glfwDestroyWindow(window_handler);
        glfwTerminate();
    }

    void Window::create_surface(VkInstance instance, VkSurfaceKHR &surface)
    {
        VK_CHECK(glfwCreateWindowSurface(instance, window_handler, nullptr, &surface), "Failed to create surface");
    }

}