// Copyright (c) Simon Kirsch 2026.

#include "VulkanCheck.h"
#include "VulkanResources.h"
#include "VulkanRHI.h"
#include "Bootstrapping/VulkanDevice.h"
#include "TranslationUtils/PipelineTranslation.h"
#include "TranslationUtils/TextureTranslations.h"

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::Pipeline, pipeline);
    device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::PipelineLayout, layout);
}

RHIGraphicsPipelineRef VulkanRHI::CreateGraphicsPipeline(RHIGraphicsPipelineDesc const &desc) {

    VkPipelineLayoutCreateInfo layoutInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };
    VkPipelineLayout layout;
    VkResult layoutResult = vkCreatePipelineLayout(device.LogicalDevice(), &layoutInfo, nullptr, &layout);
    VK_CHECK(layoutResult);

    VulkanVertexShader* vkVertexShader = ResourceCast(desc.vertexShader);
    VulkanPixelShader* vkPixelShader = ResourceCast(desc.pixelShader);

    StaticArray shaderStages = {
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vkVertexShader->Module(),
            .pName = "main"
        },
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = vkPixelShader->Module(),
            .pName = "main"
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .stencilTestEnable = VK_FALSE
    };

    VkPipelineViewportStateCreateInfo viewportState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr
    };

    VkPipelineRasterizationStateCreateInfo rasterizationState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = PipelineTranslation::ToVulkanPolygonMode(desc.raster.fillMode),
        .cullMode = PipelineTranslation::ToVulkanCullMode(desc.raster.cullMode),
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkPipelineColorBlendAttachmentState blendAttachmentState {
        .blendEnable = desc.blend.blendMode != BlendMode::Opaque,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo blendState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blendAttachmentState,
    };

    Array dynamicStates {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = dynamicStates.Size(),
        .pDynamicStates = dynamicStates.Data()
    };

    VkFormat colorFormats = ImageTranslation::ToVulkanFormat(desc.colorFormat);
    VkPipelineRenderingCreateInfo renderInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &colorFormats,
        .depthAttachmentFormat = ImageTranslation::ToVulkanFormat(desc.depthFormat)
    };

    VkGraphicsPipelineCreateInfo info {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderInfo,
        .stageCount = shaderStages.Size(),
        .pStages = shaderStages.Data(),
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = &depthStencilState,
        .pColorBlendState = &blendState,
        .pDynamicState = &dynamicState,
        .layout = layout,
        .renderPass = VK_NULL_HANDLE,
    };

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(
        device.LogicalDevice(),
        nullptr,
        1, &info,
        nullptr, &pipeline);
    VK_CHECK(result);

    return RHIGraphicsPipelineRef{ new VulkanGraphicsPipeline(&device, desc, pipeline, layout) };
}
