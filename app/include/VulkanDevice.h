//
// Created by HawkDeath on 12/29/2023.
//

#ifndef RAYCASTING_VULKANDEVICE_H
#define RAYCASTING_VULKANDEVICE_H
#include "Window.h"
#include <vulkan/vulkan.h>



namespace gfx {
    enum QueueType{
        None,
        Graphics,
        Present
    };
    struct Queue
    {
        QueueType type = QueueType::None;
        VkQueue queue_handler = VK_NULL_HANDLE;
        uint32_t index = -1u;

        static Queue find_type_of_queue(VkPhysicalDevice pd, QueueType type);
        static Queue find_present_queue(VkPhysicalDevice pd, VkSurfaceKHR &surface);

    };
    class VulkanDevice {
    public:
       explicit VulkanDevice(ray::Window &_window);
       ~VulkanDevice();

    private:
        ray::Window &m_window;
        VkInstance m_instance;
        VkSurfaceKHR  m_surface;
        VkPhysicalDevice m_physical_device;
        VkPhysicalDeviceProperties m_device_properties;
        VkDevice m_device;
        VkPhysicalDeviceFeatures m_requested_features;
        std::vector<const char*> m_requested_extensions;
        Queue m_graphcis_queue;
        Queue m_present_queue;

    };
}

#endif //RAYCASTING_VULKANDEVICE_H
