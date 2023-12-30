//
// Created by HawkDeath on 12/29/2023.
//

#ifndef RAYCASTING_VULKANDEVICE_H
#define RAYCASTING_VULKANDEVICE_H

#include "Window.h"
#include "Queue.h"
#include <vulkan/vulkan.h>

#include <memory>

namespace gfx {

    class Buffer;

    class VulkanDevice {
    public:
        explicit VulkanDevice(ray::Window &_window);

        ~VulkanDevice();

        std::shared_ptr<Buffer>
        create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags property);

        VkCommandBuffer begin_single_command_buffer();

        void end_single_command_buffer(VkCommandBuffer command);

        VkDevice device_handler() const { return m_device; }

        VkPhysicalDevice physical_device_handler() const { return m_physical_device; }

        VkSurfaceKHR surface_handler() const { return m_surface; }

        VkCommandPool graphcis_command_pool_handler() const { return m_graphics_command_pool; }

        Queue graphics_queue_handler() const { return m_graphcis_queue; }

        Queue present_queue_handler() const { return m_present_queue; }

        uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags mem_prop);
        ray::Window& window_handler() const { return m_window; }
    private:
        ray::Window &m_window;
        VkInstance m_instance;
        VkSurfaceKHR m_surface;
        VkPhysicalDevice m_physical_device;
        VkPhysicalDeviceProperties m_device_properties;
        VkPhysicalDeviceMemoryProperties m_physical_device_memory_prop;

        VkDevice m_device;
        VkPhysicalDeviceFeatures m_requested_features;
        std::vector<const char *> m_requested_extensions;
        Queue m_graphcis_queue;
        Queue m_present_queue;

        VkCommandPool m_graphics_command_pool;

    };
}

#endif //RAYCASTING_VULKANDEVICE_H
