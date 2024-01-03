//
// Created by HawkDeath on 1/3/2024.
//
#include "Renderer.h"
#include "VulkanDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Log.h"

#include <array>

namespace gfx {
    Renderer::Renderer(VulkanDevice &device) : m_device{device} {
        // TODO: handle resize here
        vkDeviceWaitIdle(m_device.device_handler());

        if (m_swap_chain == nullptr) {
            m_swap_chain = std::make_unique<SwapChain>(m_device);
        } else {
            std::shared_ptr<SwapChain> old_swap_chain = std::move(m_swap_chain);
            m_swap_chain = std::make_unique<SwapChain>(m_device, old_swap_chain);

            if (!old_swap_chain->compare_swap_formats(*m_swap_chain.get())) {
                RT_THROW("Swap chain image(or depth) format has changed");
            }
        }

        m_commands.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo cmd_buff_alloc_info = {};
        cmd_buff_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_buff_alloc_info.pNext = VK_NULL_HANDLE;
        cmd_buff_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd_buff_alloc_info.commandPool = m_device.graphcis_command_pool_handler();
        cmd_buff_alloc_info.commandBufferCount = static_cast<uint32_t>(m_commands.size());

        VK_CHECK(vkAllocateCommandBuffers(m_device.device_handler(), &cmd_buff_alloc_info, m_commands.data()),
                 "Failed to allocate command buffer");

    }

    Renderer::~Renderer() {
        vkFreeCommandBuffers(m_device.device_handler(), m_device.graphcis_command_pool_handler(),
                             static_cast<uint32_t>(m_commands.size()), m_commands.data());
        m_commands.clear();
    }


    VkCommandBuffer Renderer::begin_frame() {
        if (m_is_frame_started) { RT_THROW("Cannot start new frame while already is progress"); }

        auto res = m_swap_chain->acquireNextImage(&m_frame_idx);
        if (res == VK_ERROR_OUT_OF_DATE_KHR) {
            vkDeviceWaitIdle(m_device.device_handler());

            if (m_swap_chain == nullptr) {
                m_swap_chain = std::make_unique<SwapChain>(m_device);
            } else {
                std::shared_ptr<SwapChain> old_swap_chain = std::move(m_swap_chain);
                m_swap_chain = std::make_unique<SwapChain>(m_device, old_swap_chain);

                if (!old_swap_chain->compare_swap_formats(*m_swap_chain.get())) {
                    RT_THROW("Swap chain image(or depth) format has changed");
                }
            }
        }

        if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
            RT_THROW("Failed to acquire swap chain image");
        }

        m_is_frame_started = true;
        auto cmd = get_current_command();

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.pNext = VK_NULL_HANDLE;

        VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info), "Failed to recodring command buffer");

        return cmd;
    }

    void Renderer::end_frame() {
        if (!m_is_frame_started) {
            RT_THROW("Frame buffer is not in progress, before to call end_frame call to begin_frame");
        }
        auto cmd = get_current_command();

        VK_CHECK(vkEndCommandBuffer(cmd), "Failed to record command buffer");

        m_swap_chain->submit(&cmd, &m_frame_idx);
        m_is_frame_started = false;
        m_frame_idx = (m_frame_idx + 1u) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::begin_render_pass(VkCommandBuffer cmd) {
        if (!m_is_frame_started) { RT_THROW("Cannot begin render pass when frame is not in progress"); }

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = m_swap_chain->renderpass_handler()->renderpass_handler();
        render_pass_begin_info.framebuffer = m_swap_chain->get_framebuffer(m_frame_idx);

        render_pass_begin_info.renderArea.offset = {0, 0};
        render_pass_begin_info.renderArea.extent = m_device.window_handler().get_window_extent();

        std::array<VkClearValue, 2> clear_values = {};
        clear_values[0].color = {0.6540f, 0.7f, 0.55f, 1.0f};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_begin_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.x = viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_device.window_handler().get_window_extent().width);
        viewport.height = static_cast<float>(m_device.window_handler().get_window_extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = m_device.window_handler().get_window_extent();

        vkCmdSetViewport(cmd, 0u, 1u, &viewport);
        vkCmdSetScissor(cmd, 0u, 1u, &scissor);

    }

    void Renderer::end_render_pass(VkCommandBuffer cmd) {
        if (!m_is_frame_started) { RT_THROW("Cannot end render pass when frame is not in progress"); }
        vkCmdEndRenderPass(cmd);

    }
}
