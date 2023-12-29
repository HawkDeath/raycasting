//
// Created by HawKDeath on 12/29/2023.
//

#ifndef RAYCASTING_RAYCASTERDEMO_H
#define RAYCASTING_RAYCASTERDEMO_H

#include "Window.h"
#include "VulkanDevice.h"

namespace ray
{
    class RaycasterDemo
    {
    public:
        RaycasterDemo() = default;
        ~RaycasterDemo() = default;

        void run();

    private:
        Window m_window {"raycaster-demo", 1920u, 1080u};
        gfx::VulkanDevice m_vulkan_device{m_window};
    };
}

#endif //RAYCASTING_RAYCASTERDEMO_H
