//
// Created by HawkDeath on 12/30/2023.
//

#ifndef RAYCASTING_QUEUE_H
#define RAYCASTING_QUEUE_H

#include <vulkan/vulkan.h>
#include <limits>

namespace gfx {
    enum QueueType {
        None,
        Graphics,
        Present,
        Compute
    };

    struct Queue {
        Queue() = default;
        
        QueueType type = QueueType::None;
        VkQueue queue_handler = VK_NULL_HANDLE;
        uint32_t index = std::numeric_limits<uint32_t>::max();

        static Queue find_type_of_queue(VkPhysicalDevice pd, QueueType type);

        static Queue find_present_queue(VkPhysicalDevice pd, VkSurfaceKHR &surface);

    };
}

#endif //RAYCASTING_QUEUE_H
