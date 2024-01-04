//
// Created by HawkDeath on 1/4/2024.
//

#ifndef RAYCASTING_PIPELINE_H
#define RAYCASTING_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace gfx {
    class VulkanDevice;
    class RenderPass;

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
        std::vector<VkPipelineShaderStageCreateInfo> shaders_stage;

    };

    class Pipeline {
    public:
        explicit Pipeline(VulkanDevice &device, PipelineConfig config, VkDescriptorSetLayout descriptor_set_layout);
        virtual ~Pipeline();

        VkPipeline pipeline_handler() const { return m_pipeline; }
        const PipelineConfig& pipeline_config() const { return m_pipeline_config; }

    protected:
        VulkanDevice &m_device;
        PipelineConfig m_pipeline_config;

        VkPipelineLayout m_pipeline_layout = {};
        VkPipelineCache m_pipeline_cache = {};
        VkPipeline m_pipeline{};
    };

    class GraphicsPipeline : public Pipeline
    {
    public:
        explicit GraphicsPipeline(VulkanDevice &device, PipelineConfig config, RenderPass &render_pass, VkDescriptorSetLayout descriptor_set_layout);
        virtual ~GraphicsPipeline() = default;
    };

    class ComputePipeline : public Pipeline
    {
    public:
        explicit ComputePipeline(VulkanDevice &device, PipelineConfig config, RenderPass &render_pass, VkDescriptorSetLayout descriptor_set_layout);
        virtual ~ComputePipeline() = default;
    };

}

#endif //RAYCASTING_PIPELINE_H
