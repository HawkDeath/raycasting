//
// Created by HawkDeath on 12/31/2023.
//

#ifndef RAYCASTING_RENDERPASS_H
#define RAYCASTING_RENDERPASS_H

#include <vulkan/vulkan.h>

namespace gfx
{
    class VulkanDevice;
    class RenderPass
    {
    public:
        explicit RenderPass(VulkanDevice &device, VkFormat swapchain_format, VkFormat color_format, VkFormat depth_format);
        ~RenderPass();
        VkRenderPass renderpass_handler() const { return m_renderpass; }
    private:
        VulkanDevice &m_device;
        VkRenderPass m_renderpass;
    };
}

#endif //RAYCASTING_RENDERPASS_H
