//
// Created by HawkDeath on 12/30/2023.
//
#include "Buffer.h"
#include "Log.h"

#include "VulkanDevice.h"

namespace gfx {
    Buffer::Buffer(VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop)
            : m_device{device}, m_size{size}, m_usage{usage}, m_properties{prop} {
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = m_size;
        buffer_info.usage = m_usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK(vkCreateBuffer(m_device.device_handler(), &buffer_info, nullptr, &m_buffer),
                 "Failed to create buffer");

        VkMemoryRequirements mem_req;
        vkGetBufferMemoryRequirements(m_device.device_handler(), m_buffer, &mem_req);

        VkMemoryAllocateInfo memory_alloc_info = {};
        memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_alloc_info.allocationSize = mem_req.size;
        memory_alloc_info.memoryTypeIndex = m_device.find_memory_type(mem_req.memoryTypeBits, m_properties);

        VK_CHECK(vkAllocateMemory(m_device.device_handler(), &memory_alloc_info, nullptr, &m_buffer_memory),
                 "Failed to allocate buffer memory");
        vkBindBufferMemory(m_device.device_handler(), m_buffer, m_buffer_memory, 0u);
    }

    Buffer::~Buffer() {
        vkDestroyBuffer(m_device.device_handler(), m_buffer, nullptr);
        vkFreeMemory(m_device.device_handler(), m_buffer_memory, nullptr);
    }
}