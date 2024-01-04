//
// Created by HawkDeath on 1/4/2024.
//
#include "Pipeline.h"
#include "VulkanDevice.h"

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

    Pipeline::Pipeline(VulkanDevice &device, PipelineConfig config) : m_device{device}, m_pipeline_config{config} {

    }

    Pipeline::~Pipeline() {
        if (m_pipeline)
            vkDestroyPipeline(m_device.device_handler(), m_pipeline, nullptr);
    }
}