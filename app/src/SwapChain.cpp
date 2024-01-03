//
// Created by HawkDeath on 12/30/2023.
//
#include "SwapChain.h"
#include "VulkanDevice.h"
#include "RenderPass.h"
#include "Log.h"
#include "Image.h"

#include <array>
#include <limits>

namespace gfx {

    SwapChainResource::SwapChainResource(VulkanDevice &device, VkExtent2D extent, uint32_t mip_level,
                                         VkSampleCountFlagBits num_samples, VkFormat _format,
                                         VkImageAspectFlagBits image_aspect, VkImageTiling tiling,
                                         VkImageUsageFlags usage, VkMemoryPropertyFlags properties) : m_device{device} {

        image = m_device.create_image(extent, VK_IMAGE_TYPE_2D, image_aspect, mip_level, num_samples, _format, tiling,
                                      usage, properties);

    }

    SwapChainResource::~SwapChainResource() {
        image.reset();
    }

    SwapChain::SwapChain(gfx::VulkanDevice &device) : m_device{device}, m_image_count{0u}, m_depth_buffer{nullptr},
                                                      m_color_buffer{nullptr}, m_current_frame{0u} {
        init();
    }

    SwapChain::SwapChain(VulkanDevice &device, std::shared_ptr<gfx::SwapChain> old_swap_chain) : m_device{device},
                                                                                                 m_old_swap_chain{
                                                                                                         old_swap_chain} {
        init();
        m_old_swap_chain = nullptr;
    }

    SwapChain::~SwapChain() {

        for (uint32_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroyFence(m_device.device_handler(), m_in_flight_fences[i], nullptr);
            vkDestroySemaphore(m_device.device_handler(), m_render_finished_semaphores[i], nullptr);
            vkDestroySemaphore(m_device.device_handler(), m_image_available_semaphores[i], nullptr);
        }

        m_depth_buffer.reset();
        m_color_buffer.reset();

        for (auto framebuffer: m_framebuffers) {
            vkDestroyFramebuffer(m_device.device_handler(), framebuffer, nullptr);
        }

        for (auto img_view: m_swapchain_image_views) {
            vkDestroyImageView(m_device.device_handler(), img_view, nullptr);
        }
        vkDestroySwapchainKHR(m_device.device_handler(), m_swapchain, nullptr);
    }

    VkResult SwapChain::acquireNextImage(uint32_t *image_idx) {
        vkWaitForFences(m_device.device_handler(), 1u, &m_in_flight_fences[m_current_frame], VK_TRUE,
                        std::numeric_limits<uint64_t>::max());
        VkResult res = vkAcquireNextImageKHR(m_device.device_handler(), m_swapchain,
                                             std::numeric_limits<uint64_t>::max(),
                                             m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE,
                                             image_idx);
        if (res == VK_ERROR_OUT_OF_DATE_KHR ||
            (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)) // TODO: handle resize - VK_ERROR_OUT_OF_DATE_KHR event
        {
            RT_THROW("Failed to acquire swap chain image");
        }
        return res;
    }

    void SwapChain::submit(const VkCommandBuffer *cmd, uint32_t *image_idx) {

        if (m_images_in_flight[*image_idx] != VK_NULL_HANDLE)
        {
            vkWaitForFences(m_device.device_handler(), 1u, &m_images_in_flight[*image_idx], VK_TRUE, std::numeric_limits<uint64_t>::max());
        }
        m_images_in_flight[*image_idx] = m_in_flight_fences[m_current_frame];

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = VK_NULL_HANDLE;

        VkSemaphore wait_samphores[] = {m_image_available_semaphores[m_current_frame]};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};

        submit_info.waitSemaphoreCount = 1u;
        submit_info.pWaitSemaphores = wait_samphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1u;
        submit_info.pCommandBuffers = cmd;

        VkSemaphore signal_semaphores[] = {m_render_finished_semaphores[m_current_frame]};
        submit_info.signalSemaphoreCount = 1u;
        submit_info.pSignalSemaphores = signal_semaphores;

        vkResetFences(m_device.device_handler(), 1u, &m_in_flight_fences[m_current_frame]);
        VK_CHECK(vkQueueSubmit(m_device.graphics_queue_handler().queue_handler, 1u, &submit_info,
                               m_in_flight_fences[m_current_frame]), "Failed to submit draw cmd buffer");
        // present frame on screen

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = VK_NULL_HANDLE;

        present_info.waitSemaphoreCount = 1u;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {m_swapchain};
        present_info.swapchainCount = 1u;
        present_info.pSwapchains = swapchains;

        present_info.pImageIndices = image_idx;

        vkQueuePresentKHR(m_device.present_queue_handler().queue_handler, &present_info);
        // TODO: handle errors and resize

        m_current_frame = (m_current_frame + 1u) % MAX_FRAMES_IN_FLIGHT;
    }

    bool SwapChain::compare_swap_formats(const gfx::SwapChain &swapchain) const {
        return ((swapchain.m_color_buffer->image->format() == m_color_buffer->image->format()) &&
                (swapchain.m_depth_buffer->image->format() == swapchain.m_depth_buffer->image->format()));
    }

    void SwapChain::init() {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.physical_device_handler(), m_device.surface_handler(),
                                                  &m_surface_capabilities);
        uint32_t format_counts = 0u;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.physical_device_handler(), m_device.surface_handler(),
                                             &format_counts, nullptr);

        if (format_counts == 0u) RT_THROW("Cannot find any surface format");
        m_supported_formats.clear();
        m_supported_formats.resize(format_counts);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.physical_device_handler(), m_device.surface_handler(),
                                             &format_counts, m_supported_formats.data());

        uint32_t present_modes = 0u;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.physical_device_handler(), m_device.surface_handler(),
                                                  &present_modes, nullptr);

        if (present_modes == 0u) RT_THROW("Cannot find any present modes");
        m_supported_modes.clear();
        m_supported_modes.resize(present_modes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.physical_device_handler(), m_device.surface_handler(),
                                                  &present_modes, m_supported_modes.data());

        VkExtent2D extent = m_device.window_handler().get_window_extent(); // base on window resolution
        VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // default
        VkSurfaceFormatKHR surface_format = m_supported_formats[0];

        for (const auto &avail_format: m_supported_formats) {
            if (avail_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                avail_format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
                surface_format = avail_format;
                break;
            }
        }

        for (const auto &avail_present_modes: m_supported_modes) {
            if (avail_present_modes == VK_PRESENT_MODE_MAILBOX_KHR) {
                present_mode = avail_present_modes;
                break;
            }
        }

        m_image_count = m_surface_capabilities.minImageCount + 1u;
        if (m_surface_capabilities.maxImageCount > 0u && m_image_count > m_surface_capabilities.maxImageCount) {
            m_image_count = m_surface_capabilities.maxImageCount;
        }
        // create swapchain object
        VkSwapchainCreateInfoKHR swapchain_create_info = {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = m_device.surface_handler();
        swapchain_create_info.minImageCount = m_image_count;
        swapchain_create_info.imageFormat = surface_format.format;
        swapchain_create_info.imageColorSpace = surface_format.colorSpace;
        swapchain_create_info.imageExtent = extent;
        swapchain_create_info.imageArrayLayers = 1u;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        uint32_t queue_families[] = {m_device.graphics_queue_handler().index, m_device.present_queue_handler().index};

        if (m_device.graphics_queue_handler().index != m_device.present_queue_handler().index) {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.queueFamilyIndexCount = 2u;
            swapchain_create_info.pQueueFamilyIndices = queue_families;
        } else {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        swapchain_create_info.preTransform = m_surface_capabilities.currentTransform;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.clipped = VK_TRUE;

        swapchain_create_info.oldSwapchain = m_old_swap_chain == nullptr ? VK_NULL_HANDLE : m_old_swap_chain->m_swapchain;

        VK_CHECK(vkCreateSwapchainKHR(m_device.device_handler(), &swapchain_create_info, nullptr, &m_swapchain),
                 "Failed to create swapchain");
        LOG("Swapchain has been created");

        vkGetSwapchainImagesKHR(m_device.device_handler(), m_swapchain, &m_image_count, nullptr);
        m_swapchain_images.resize(m_image_count);
        vkGetSwapchainImagesKHR(m_device.device_handler(), m_swapchain, &m_image_count, m_swapchain_images.data());

        m_format = surface_format.format;
        m_extent = extent;

        // create swapchain images view
        m_swapchain_image_views.resize(m_swapchain_images.size());
        for (uint32_t i = 0u; i < m_swapchain_image_views.size(); ++i) {
            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = VK_NULL_HANDLE;
            image_view_create_info.image = m_swapchain_images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = m_format;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0u;
            image_view_create_info.subresourceRange.levelCount = 1u;
            image_view_create_info.subresourceRange.baseArrayLayer = 0u;
            image_view_create_info.subresourceRange.layerCount = 1u;
            VK_CHECK(vkCreateImageView(m_device.device_handler(), &image_view_create_info, nullptr,
                                       &m_swapchain_image_views[i]), "Failed to create swapchain image view");
        }

        // create color buffer
        m_color_buffer = std::make_unique<SwapChainResource>(m_device, m_extent, 1u, VK_SAMPLE_COUNT_1_BIT, m_format,
                                                             VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL,
                                                             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // create depth buffer
        m_depth_buffer = std::make_unique<SwapChainResource>(m_device, m_extent, 1u, VK_SAMPLE_COUNT_1_BIT,
                                                             VK_FORMAT_D32_SFLOAT, // TODO: some GPUs may not supported this format
                                                             VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_TILING_OPTIMAL,
                                                             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_renderpass = std::make_shared<RenderPass>(m_device, m_format, m_color_buffer->image->format(),
                                                    m_depth_buffer->image->format());

        // create framebuffers
        m_framebuffers.resize(m_swapchain_image_views.size());
        for (uint32_t i = 0u; i < m_framebuffers.size(); ++i) {
            std::array<VkImageView, 2> attachments = {
                    m_swapchain_image_views[i],
                    m_depth_buffer->image->image_view_handler()

            };

            VkFramebufferCreateInfo framebuffer_create_info = {};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.pNext = VK_NULL_HANDLE;
            framebuffer_create_info.renderPass = m_renderpass->renderpass_handler();
            framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebuffer_create_info.pAttachments = attachments.data();
            framebuffer_create_info.width = m_extent.width;
            framebuffer_create_info.height = m_extent.height;
            framebuffer_create_info.layers = 1u;

            VK_CHECK(vkCreateFramebuffer(m_device.device_handler(), &framebuffer_create_info, nullptr,
                                         &m_framebuffers[i]), "Failed to create framebuffer");
        }
        LOG("Framebuffer has been created. No. of frames {}", m_framebuffers.size());


        m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
        m_images_in_flight.resize(m_image_count, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphore_create_info = {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_create_info.pNext = VK_NULL_HANDLE;

        VkFenceCreateInfo fence_create_info = {};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.pNext = VK_NULL_HANDLE;
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_CHECK(vkCreateSemaphore(m_device.device_handler(), &semaphore_create_info, nullptr,
                                       &m_render_finished_semaphores[i]), "Failed to create render semaphore");
        }
        for (uint32_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_CHECK(vkCreateSemaphore(m_device.device_handler(), &semaphore_create_info, nullptr,
                                       &m_image_available_semaphores[i]), "Failed to create image semaphore");
        }
        for (uint32_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_CHECK(vkCreateFence(m_device.device_handler(), &fence_create_info, nullptr, &m_in_flight_fences[i]),
                     "Failed to fence");
        }
    }


}