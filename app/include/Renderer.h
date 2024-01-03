//
// Created by HawkDeath on 1/3/2024.
//

#ifndef RAYCASTING_RENDERER_H
#define RAYCASTING_RENDERER_H

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace gfx
{
    class SwapChain;
    class VulkanDevice;
class Renderer
{
public:
    explicit Renderer(VulkanDevice &device);
    ~Renderer();

    VkCommandBuffer get_current_command() const { return m_commands[m_frame_idx]; }

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_render_pass(VkCommandBuffer cmd);
    void end_render_pass(VkCommandBuffer cmd);

    uint32_t get_frame_index() const { return m_frame_idx; }
    bool is_frame_in_progress() const  {return m_is_frame_started;}

private:
    VulkanDevice &m_device;

    std::unique_ptr<SwapChain> m_swap_chain;
    std::vector<VkCommandBuffer> m_commands;

    uint32_t  m_frame_idx = {0u};
    bool m_is_frame_started = {false};
};
}

#endif //RAYCASTING_RENDERER_H
