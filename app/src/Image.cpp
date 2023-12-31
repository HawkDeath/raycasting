//
// Created by HawkDeath on 12/31/2023.
//

#include "Image.h"
#include "VulkanDevice.h"
#include "Log.h"

namespace gfx {
    Image::Image(VulkanDevice &device, VkExtent2D resolution, VkImageType type, VkImageAspectFlags aspect_flag,
                 uint32_t mip_level, VkSampleCountFlagBits sample_count,
                 VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags property)
            : m_device{device}, m_format{format} {
        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = type;
        image_create_info.extent.width = resolution.width;
        image_create_info.extent.height = resolution.height;
        image_create_info.extent.depth = 1u; // TODO: add support for 3d textures
        image_create_info.mipLevels = mip_level;
        image_create_info.arrayLayers = 1u;
        image_create_info.format = format;
        image_create_info.tiling = tiling;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = usage;
        image_create_info.samples = sample_count;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK(vkCreateImage(m_device.device_handler(), &image_create_info, nullptr, &m_image),
                 "failed to create image!");


        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device.device_handler(), m_image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = m_device.find_memory_type(memRequirements.memoryTypeBits, property);

        VK_CHECK(vkAllocateMemory(device.device_handler(), &allocInfo, nullptr, &m_memory_device),
                 "failed to allocate image memory!");


        vkBindImageMemory(device.device_handler(), m_image, m_memory_device, 0u);

        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = m_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = format;
        image_view_create_info.subresourceRange.aspectMask = aspect_flag;
        image_view_create_info.subresourceRange.baseMipLevel = 0u;
        image_view_create_info.subresourceRange.levelCount = mip_level;
        image_view_create_info.subresourceRange.baseArrayLayer = 0u;
        image_view_create_info.subresourceRange.layerCount = 1u;

        VK_CHECK(vkCreateImageView(m_device.device_handler(), &image_view_create_info, nullptr, &m_image_view),
                 "failed to create texture image view!");

    }

    Image::~Image() {
        vkDestroyImageView(m_device.device_handler(), m_image_view, nullptr);
        vkDestroyImage(m_device.device_handler(), m_image, nullptr);
        vkFreeMemory(m_device.device_handler(), m_memory_device, nullptr);
    }

}
