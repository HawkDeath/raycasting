#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

int main() {
    glfwSetErrorCallback([](int code, const char *desc) -> void {spdlog::error("[GLFW_error] {}: {}", code, desc);});
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "raycasting-test", nullptr, nullptr);

    uint32_t api_version;
    vkEnumerateInstanceVersion(&api_version);
    spdlog::info("Vulkan version {}.{}", VK_VERSION_MAJOR(api_version), VK_VERSION_MINOR(api_version));

    auto res = glm::vec4(1.0f) * glm::mat4(1.0f);
    

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    if (window)
        glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}