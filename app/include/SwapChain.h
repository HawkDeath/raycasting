//
// Created by HawkDeath on 12/30/2023.
//

#ifndef RAYCASTING_SWAPCHAIN_H
#define RAYCASTING_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>


namespace gfx {
    class VulkanDevice;

    class Image;

    class RenderPass;

    struct SwapChainResource // TODO: change name; object represent the depth buffer and color buffer resource
    {
        explicit SwapChainResource(VulkanDevice &device, VkExtent2D extent, uint32_t mip_level,
                                   VkSampleCountFlagBits num_samples, VkFormat _format,
                                   VkImageAspectFlagBits image_aspect, VkImageTiling tiling, VkImageUsageFlags usage,
                                   VkMemoryPropertyFlags properties);

        ~SwapChainResource();

        std::shared_ptr<Image> image;
    private:
        VulkanDevice &m_device;
    };

    class SwapChain {
    public:
        static const uint32_t MAX_FRAMES_IN_FLIGHT = 2u;

        explicit SwapChain(VulkanDevice &device);
        explicit SwapChain(VulkanDevice &device, std::shared_ptr<SwapChain> old_swap_chain);

        ~SwapChain();

        VkResult acquireNextImage(uint32_t *image_idx);

        void submit(const VkCommandBuffer *cmd, uint32_t *image_idx);


        VkSwapchainKHR swapchain_handler() const { return m_swapchain; }

        std::shared_ptr<RenderPass> renderpass_handler() const { return m_renderpass; }

        [[nodiscard]] VkFramebuffer get_framebuffer(const uint32_t &idx) const { return m_framebuffers[idx]; }

        bool compare_swap_formats(const SwapChain &swapchain) const;

    private:
        void init();

    private:
        VulkanDevice &m_device;
        VkSwapchainKHR m_swapchain;
        std::shared_ptr<SwapChain> m_old_swap_chain;
        uint32_t m_image_count;
        VkFormat m_format;
        VkExtent2D m_extent;

        std::unique_ptr<SwapChainResource> m_depth_buffer;
        std::unique_ptr<SwapChainResource> m_color_buffer;
        std::shared_ptr<RenderPass> m_renderpass;
        VkSurfaceCapabilitiesKHR m_surface_capabilities;
        std::vector<VkSurfaceFormatKHR> m_supported_formats;
        std::vector<VkPresentModeKHR> m_supported_modes;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
        std::vector<VkFramebuffer> m_framebuffers;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_in_flight_fences;
        std::vector<VkFence> m_images_in_flight;
        size_t m_current_frame = 0u;

    };
}

#endif //RAYCASTING_SWAPCHAIN_H
