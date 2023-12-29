//
// Created by HawkDeath on 12/29/2023.
//

#ifndef RAYCASTING_WINDOW_H
#define RAYCASTING_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

namespace ray
{
    class Window
    {
    public:
        explicit Window(const std::string &_title, const uint32_t &_width, const uint32_t &_height);
        ~Window();

        [[nodiscard]]  bool is_running() noexcept { return !glfwWindowShouldClose(window_handler); }

        [[nodiscard]] VkExtent2D get_window_extent() const { return {width, height};}

        void create_surface(VkInstance instance, VkSurfaceKHR &surface);

    private:
        GLFWwindow *window_handler{};
        uint32_t width {};
        uint32_t height {};

    };
}

#endif //RAYCASTING_WINDOW_H
