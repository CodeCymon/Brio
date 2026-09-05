// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>

#include "RHIResources.h"

struct VulkanImageStateInfo {
    VkImageLayout layout;
    VkPipelineStageFlags2 stage;
    VkAccessFlags2 access;
};


namespace ImageTranslation {
    VkImageType ToVulkanDimensionType(TextureDimension dimension);
    VkImageUsageFlags ToVulkanUsageFlags(TextureUsage usage);
    VkFormat ToVulkanFormat(PixelFormat format);
    VkImageAspectFlags ToVulkanImageAspectFlags(PixelFormat format);
    VkSampleCountFlagBits ToVulkanSampleCount(u32 sampleCount);

    VulkanImageStateInfo ToVulkanImageState(RHIResourceState state);

    VkImageCreateInfo CreateInfoFromDesc(RHITextureDesc const& desc);
};

