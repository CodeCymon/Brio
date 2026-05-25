#pragma once

#include <vulkan/vulkan_core.h>

#include "RHIResources.h"


class FVulkanTexture : public FRHITexture {
public:
    FVulkanTexture(FTextureDesc const& desc) : FRHITexture(desc) {}

    VkImage image {VK_NULL_HANDLE};
    VkImageView view {VK_NULL_HANDLE};
    VkDeviceMemory memory {VK_NULL_HANDLE};
    bool bExternalMemory {false};
};


class FVulkanShader : public FRHIShader {
public:
    FVulkanShader(FShaderDesc const& desc) : FRHIShader(desc) {}

    VkShaderModule module {VK_NULL_HANDLE};
};


class FVulkanGraphicsPipeline : public FRHIGraphicsPipeline {
public:
    FVulkanGraphicsPipeline(FGraphicsPipelineDesc const& desc) : FRHIGraphicsPipeline(desc) {}

    VkPipeline pipeline {VK_NULL_HANDLE};
    VkPipelineLayout layout {VK_NULL_HANDLE};
};