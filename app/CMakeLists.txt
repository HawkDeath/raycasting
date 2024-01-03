cmake_minimum_required(VERSION 3.25)


set(GFX_HEADERS
        ./include/Renderer.h
        ./include/Image.h
        ./include/RenderPass.h
        ./include/SwapChain.h
        ./include/Buffer.h
        ./include/Queue.h
        ./include/VulkanDevice.h)

set(GFX_SOURCES
        ./src/Renderer.cpp
        ./src/Image.cpp
        ./src/RenderPass.cpp
        ./src/SwapChain.cpp
        ./src/Buffer.cpp
        ./src/Queue.cpp
        ./src/VulkanDevice.cpp)

set(APP_HEADERS
        ./include/RaycasterDemo.h
        ./include/Log.h
        ./include/Window.h)
set(APP_SRC
        ./src/RaycasterDemo.cpp
        ./src/Window.cpp
        main.cpp)

set(APP_CODE ${APP_HEADERS} ${APP_SRC} ${GFX_HEADERS} ${GFX_SOURCES})
add_executable(raycasting ${APP_CODE})
target_link_libraries(raycasting PRIVATE glfw glm Vulkan::Vulkan spdlog)
target_include_directories(raycasting PRIVATE include)