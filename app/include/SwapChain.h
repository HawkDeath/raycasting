//
// Created by HawkDeath on 12/30/2023.
//

#ifndef RAYCASTING_SWAPCHAIN_H
#define RAYCASTING_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>


namespace gfx
{
    class VulkanDevice;

    struct SwapChainResource // TODO: change name; object represent the depth buffer and color buffer resource
    {
        explicit SwapChainResource(VulkanDevice &device, VkExtent2D extent, uint32_t mip_level, VkSampleCountFlags num_samples, VkFormat _format, VkImageAspectFlagBits image_aspect, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~SwapChainResource();
        VkImage image;
        VkImageView image_view;
        VkDeviceMemory device_memory;
        VkFormat format;
    private:
        VulkanDevice &m_device;
    };
    class SwapChain
    {
    public:
       explicit SwapChain(VulkanDevice &device);
       ~SwapChain();
        VkSwapchainKHR swapchain_handler() const { return m_swapchain; }

    private:
        VulkanDevice &m_device;
        VkRenderPass m_render_pass; // TODO: create
        VkSwapchainKHR m_swapchain;
        uint32_t m_image_count;
        VkFormat m_format;
        VkExtent2D m_extent;

        std::unique_ptr<SwapChainResource> m_depth_buffer;
        std::unique_ptr<SwapChainResource> m_color_buffer;

        VkSurfaceCapabilitiesKHR m_surface_capabilities;
        std::vector<VkSurfaceFormatKHR> m_supported_formats;
        std::vector<VkPresentModeKHR> m_supported_modes;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
        std::vector<VkFramebuffer> m_framebuffers;
    };
}

#endif //RAYCASTING_SWAPCHAIN_H
