//
// Created by HawkDeath on 1/4/2024.
//

#ifndef RAYCASTING_PIPELINE_H
#define RAYCASTING_PIPELINE_H

#include <vulkan/vulkan.h>

namespace gfx {
    class VulkanDevice;

    struct PipelineConfig {
        static void default_config(PipelineConfig &pipeline);

        PipelineConfig() = default;

        VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
        VkPipelineViewportStateCreateInfo viewport_info = {};
        VkPipelineRasterizationStateCreateInfo rasterization_info = {};
        VkPipelineMultisampleStateCreateInfo multisample_info = {};
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
        VkPipelineColorBlendAttachmentState color_blend_attachment_info = {};
        VkPipelineColorBlendStateCreateInfo color_blend_info = {};
        VkPipelineDynamicStateCreateInfo dynamic_info = {};
        VkPipelineLayoutCreateInfo layout_info = {};
    };

    class Pipeline {
    public:
        explicit Pipeline(VulkanDevice &device, PipelineConfig config);
        virtual ~Pipeline();

    protected:
        VulkanDevice &m_device;
        PipelineConfig m_pipeline_config;

        VkPipeline m_pipeline{};
    };

}

#endif //RAYCASTING_PIPELINE_H
