

set(APP_SRC main.cpp)

add_executable(raycasting ${APP_SRC})
target_link_libraries(raycasting PRIVATE glfw glm Vulkan::Vulkan spdlog)