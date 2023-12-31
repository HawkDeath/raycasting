//
// Created by HawkDeath on 12/31/2023.
//

#ifndef RAYCASTING_IMAGE_H
#define RAYCASTING_IMAGE_H

#include <vulkan/vulkan.h>

namespace gfx {
    class VulkanDevice;

    class Image {
    public:
        explicit Image(VulkanDevice &device, VkExtent2D resolution, VkImageType type, VkImageAspectFlags aspect_flag,
                       uint32_t mip_level, VkSampleCountFlagBits sample_count,
                       VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags property);

        ~Image();

        VkFormat format() const { return m_format; }

        VkImage image_handler() const { return m_image; }

        VkImageView image_view_handler() const { return m_image_view; }

    private:
        VulkanDevice &m_device;
        VkImage m_image;
        VkImageView m_image_view;
        VkDeviceMemory m_memory_device;
        VkFormat m_format;

    };
}

#endif //RAYCASTING_IMAGE_H
