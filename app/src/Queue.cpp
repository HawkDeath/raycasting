//
// Created by HawkDeath on 12/30/2023.
//
#include "Queue.h"
#include <vector>

namespace gfx {
    Queue Queue::find_type_of_queue(VkPhysicalDevice pd, QueueType type) {
        uint32_t queue_family_count = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, queue_families.data());
        uint32_t idx = 0u;
        for (const auto &queue: queue_families) {
            if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT && type == QueueType::Graphics) {
                Queue graphics_queue;
                graphics_queue.type = QueueType::Graphics;
                graphics_queue.index = idx;
                return graphics_queue;
            }

            if (queue.queueFlags & VK_QUEUE_COMPUTE_BIT && type == QueueType::Compute) {
                Queue compute_queue;
                compute_queue.type = QueueType::Compute;
                compute_queue.index = idx;
                return compute_queue;
            }

            ++idx;
        }
        return Queue(); // invalid queue
    }

    Queue Queue::find_present_queue(VkPhysicalDevice pd, VkSurfaceKHR &surface) {
        uint32_t queue_family_count = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, queue_families.data());
        uint32_t idx = 0u;
        for (const auto &queue: queue_families) {
            VkBool32 is_present_supported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(pd, idx, surface, &is_present_supported);
            if (is_present_supported) {
                Queue present_queue;
                present_queue.type = QueueType::Present;
                present_queue.index = idx;
                return present_queue;
            }
            ++idx;
        }
        return Queue(); // invalid queue
    }


}