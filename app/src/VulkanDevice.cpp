//
// Created by HawkDeath on 12/29/2023.
//
#include "VulkanDevice.h"
#include "Log.h"
#include <set>


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


    VulkanDevice::VulkanDevice(ray::Window &_window) : m_window{_window} {
        // create instance
        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pNext = VK_NULL_HANDLE;
        app_info.apiVersion = VK_MAKE_VERSION(1, 3, 0);
        app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        app_info.pApplicationName = "raycastring_app";
        app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        app_info.pEngineName = "raycastring_engine";

        auto extensions = m_window.required_extensions();
        LOG("Available extensions:");
        for (auto &e: extensions) {
            LOG("\t{}", e);
        }

        VkInstanceCreateInfo instance_create_info = {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pNext = VK_NULL_HANDLE;
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.flags = 0u;
        instance_create_info.enabledLayerCount = 0u;
        instance_create_info.ppEnabledLayerNames = VK_NULL_HANDLE;
        instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instance_create_info.ppEnabledExtensionNames = extensions.data();
        VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &m_instance), "Failed to create instance");

        // create surface
        m_window.create_surface(m_instance, m_surface);

        // pick physical device
        uint32_t physical_device_count = 0u;
        vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr);
        if (physical_device_count == 0u) {
            RT_THROW("Cannot find GPU(s) with Vulkan support");
        }

        std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
        vkEnumeratePhysicalDevices(m_instance, &physical_device_count, physical_devices.data());

        for (const auto &pd: physical_devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(pd, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                m_physical_device = pd;
                break;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE)
            m_physical_device = physical_devices[0];

        vkGetPhysicalDeviceProperties(m_physical_device, &m_device_properties);
        LOG("Choosen: {}", m_device_properties.deviceName);
        LOG("Vulkan API version supported {}.{}.{}", VK_API_VERSION_MAJOR(m_device_properties.apiVersion),
            VK_API_VERSION_MINOR(m_device_properties.apiVersion), VK_API_VERSION_PATCH(m_device_properties.apiVersion));

        // create logical device
        m_present_queue = Queue::find_present_queue(m_physical_device, m_surface);
        m_graphcis_queue = Queue::find_type_of_queue(m_physical_device, QueueType::Graphics);

        std::set<uint32_t> unique_queue_families = {m_graphcis_queue.index, m_present_queue.index};
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        const float prio = 1.0f;
        for (uint32_t qf : unique_queue_families) {
            VkDeviceQueueCreateInfo device_queue_create_info = {};
            device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_info.pNext = VK_NULL_HANDLE;
            device_queue_create_info.queueFamilyIndex = qf;
            device_queue_create_info.queueCount = 1u;
            device_queue_create_info.pQueuePriorities = &prio;
            queue_create_infos.push_back(device_queue_create_info);
        }

        // TODO: refactor and make reconfigurable
        m_requested_features.samplerAnisotropy = VK_TRUE;
        m_requested_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo device_create_info = {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = VK_NULL_HANDLE;
        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pQueueCreateInfos = queue_create_infos.data();

        device_create_info.pEnabledFeatures = &m_requested_features;
        device_create_info.enabledExtensionCount = static_cast<uint32_t>(m_requested_extensions.size());
        device_create_info.ppEnabledExtensionNames = m_requested_extensions.data();

        device_create_info.enabledLayerCount = 0u;

        VK_CHECK(vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device), "Failed to create physical device");

        vkGetDeviceQueue(m_device, m_graphcis_queue.index, 0u, &m_graphcis_queue.queue_handler);
        vkGetDeviceQueue(m_device, m_present_queue.index, 0u, &m_present_queue.queue_handler);

        if (m_graphcis_queue.queue_handler == VK_NULL_HANDLE) RT_THROW("Invalid graphic queue");
        if (m_present_queue.queue_handler == VK_NULL_HANDLE) RT_THROW("Invalid present queue");
        LOG("Logical device has been created");
    }

    VulkanDevice::~VulkanDevice() {


        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }
}