//
// Created by HawkDeath on 12/30/2023.
//

#ifndef RAYCASTING_BUFFER_H
#define RAYCASTING_BUFFER_H

#include <vulkan/vulkan.h>

namespace gfx {
    class VulkanDevice;

    class Buffer {
    public:
        explicit Buffer(VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop);

        ~Buffer();

        VkBuffer get_buffer() const { return m_buffer; }

        VkDeviceMemory get_device_memory() const { return m_buffer_memory; }

        VkDeviceSize get_size() const { return m_size; }

        VkBufferUsageFlags get_usage_flag() const { return m_usage; }

        VkMemoryPropertyFlags get_memory_property_flag() const { return m_properties; }


    private:
        VulkanDevice &m_device;
        VkBuffer m_buffer;
        VkDeviceMemory m_buffer_memory;
        VkDeviceSize m_size;
        VkBufferUsageFlags m_usage;
        VkMemoryPropertyFlags m_properties;
    };
}

#endif //RAYCASTING_BUFFER_H
