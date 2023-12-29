cmake_minimum_required(VERSION 3.25)

set(APP_HEADERS
        ./include/RaycasterDemo.h
        ./include/Log.h
        ./include/Window.h )
set(APP_SRC
        ./src/RaycasterDemo.cpp
        ./src/Window.cpp
        main.cpp)

set(APP_CODE ${APP_HEADERS} ${APP_SRC})
add_executable(raycasting ${APP_CODE})
target_link_libraries(raycasting PRIVATE glfw glm Vulkan::Vulkan spdlog)
target_include_directories(raycasting PRIVATE include)