//
// Created by HawkDeath on 12/29/2023.
//
#include "RaycasterDemo.h"
#include <vulkan/vulkan.h>
#include "Log.h"
#include "RenderPass.h"

#include <array>

namespace ray {
    void RaycasterDemo::run() {

        while (m_window.is_running()) {
            m_window.process_inputs();

            if (auto cmd = m_renderer.begin_frame())
            {
                m_renderer.begin_render_pass(cmd);
                m_renderer.end_render_pass(cmd);
                m_renderer.end_frame();
            }

        }
        m_vulkan_device.waitQueueIdle();
    }
}