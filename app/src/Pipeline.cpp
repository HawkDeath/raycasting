//
// Created by HawkDeath on 1/4/2024.
//
#include "Pipeline.h"

#include <utility>
#include "VulkanDevice.h"
#include "RenderPass.h"
#include "Log.h"

namespace gfx {
    void PipelineConfig::default_config(PipelineConfig &pipeline) {
        pipeline.vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipeline.vertex_input_info.pNext = VK_NULL_HANDLE;

        pipeline.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipeline.input_assembly_info.pNext = VK_NULL_HANDLE;
        pipeline.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipeline.input_assembly_info.primitiveRestartEnable = VK_FALSE;

        pipeline.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipeline.viewport_info.pNext = VK_NULL_HANDLE;
        pipeline.viewport_info.scissorCount = 1u;
        pipeline.viewport_info.viewportCount = 1u;

        pipeline.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipeline.rasterization_info.pNext = VK_NULL_HANDLE;
        pipeline.rasterization_info.depthClampEnable = VK_TRUE;
        pipeline.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        pipeline.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
        pipeline.rasterization_info.lineWidth = 1.0f;
        pipeline.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
        pipeline.rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipeline.rasterization_info.depthBiasClamp = VK_FALSE;

        pipeline.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipeline.multisample_info.pNext = VK_NULL_HANDLE;
        pipeline.multisample_info.sampleShadingEnable = VK_FALSE;
        pipeline.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        pipeline.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipeline.depth_stencil_info.pNext = VK_NULL_HANDLE;
        pipeline.depth_stencil_info.depthTestEnable = VK_TRUE;
        pipeline.depth_stencil_info.depthWriteEnable = VK_TRUE;
        pipeline.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
        pipeline.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        pipeline.depth_stencil_info.stencilTestEnable = VK_FALSE;

        pipeline.color_blend_attachment_info.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        pipeline.color_blend_attachment_info.blendEnable = VK_FALSE;

        pipeline.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipeline.color_blend_info.pNext = VK_NULL_HANDLE;
        pipeline.color_blend_info.logicOpEnable = VK_FALSE;
        pipeline.color_blend_info.logicOp = VK_LOGIC_OP_COPY;
        pipeline.color_blend_info.attachmentCount = 1u;
        pipeline.color_blend_info.pAttachments = &pipeline.color_blend_attachment_info;
        pipeline.color_blend_info.blendConstants[0] = 0.0f;
        pipeline.color_blend_info.blendConstants[1] = 0.0f;
        pipeline.color_blend_info.blendConstants[2] = 0.0f;
        pipeline.color_blend_info.blendConstants[3] = 0.0f;

        pipeline.dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipeline.dynamic_info.pNext = VK_NULL_HANDLE;

        pipeline.layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline.layout_info.pNext = VK_NULL_HANDLE;
    }

    Pipeline::Pipeline(VulkanDevice &device, PipelineConfig config, VkDescriptorSetLayout descriptor_set_layout)
            : m_device{device}, m_pipeline_config{std::move(config)} {
        VkPipelineLayoutCreateInfo pipeline_layout_info = {};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.pNext = VK_NULL_HANDLE;
        pipeline_layout_info.setLayoutCount = 1u;
        pipeline_layout_info.pSetLayouts = &descriptor_set_layout;

        VK_CHECK(vkCreatePipelineLayout(m_device.device_handler(), &pipeline_layout_info, nullptr, &m_pipeline_layout),
                 "Failed to create pipeline layout");

        VkPipelineCacheCreateInfo pipeline_cache_info = {};
        pipeline_cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipeline_cache_info.pNext = VK_NULL_HANDLE;
        pipeline_cache_info.initialDataSize = 0u;
        pipeline_cache_info.pInitialData = VK_NULL_HANDLE;

        VK_CHECK(vkCreatePipelineCache(m_device.device_handler(), &pipeline_cache_info, nullptr, &m_pipeline_cache),
                 "Failed to create pipeline cache");
    }

    Pipeline::~Pipeline() {
        vkDestroyPipelineLayout(m_device.device_handler(), m_pipeline_layout, nullptr);
        if (m_pipeline_cache)
            vkDestroyPipelineCache(m_device.device_handler(), m_pipeline_cache, nullptr);
        if (m_pipeline)
            vkDestroyPipeline(m_device.device_handler(), m_pipeline, nullptr);
    }

    GraphicsPipeline::GraphicsPipeline(VulkanDevice &device, PipelineConfig config, RenderPass &render_pass,
                                       VkDescriptorSetLayout descriptor_set_layout) : Pipeline(device,
                                                                                               std::move(config),
                                                                                               descriptor_set_layout) {

        VkGraphicsPipelineCreateInfo graphics_pipeline_info = {};
        graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_info.pNext = VK_NULL_HANDLE;

        graphics_pipeline_info.stageCount = static_cast<uint32_t>(m_pipeline_config.shaders_stage.size());
        graphics_pipeline_info.pStages = m_pipeline_config.shaders_stage.data();

        graphics_pipeline_info.pVertexInputState = &m_pipeline_config.vertex_input_info;
        graphics_pipeline_info.pInputAssemblyState = &m_pipeline_config.input_assembly_info;
        graphics_pipeline_info.pViewportState = &m_pipeline_config.viewport_info;
        graphics_pipeline_info.pRasterizationState = &m_pipeline_config.rasterization_info;
        graphics_pipeline_info.pMultisampleState = &m_pipeline_config.multisample_info;
        graphics_pipeline_info.pDepthStencilState = &m_pipeline_config.depth_stencil_info;
        graphics_pipeline_info.pColorBlendState = &m_pipeline_config.color_blend_info;
        graphics_pipeline_info.pDynamicState = &m_pipeline_config.dynamic_info;
        graphics_pipeline_info.layout = m_pipeline_layout;
        graphics_pipeline_info.renderPass = render_pass.renderpass_handler();
        graphics_pipeline_info.subpass = 0u;
        graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        VK_CHECK(vkCreateGraphicsPipelines(m_device.device_handler(), m_pipeline_cache, 1u, &graphics_pipeline_info, nullptr, &m_pipeline), "Failed to create graphics pipeline");

    }

}