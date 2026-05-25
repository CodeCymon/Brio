#pragma once

#include <vulkan/vulkan_core.h>

#include "RHIResources.h"

struct FTextureDesc;

class FVulkanTexture : public FRHITexture {
public:
    FVulkanTexture(FTextureDesc const& desc) : FRHITexture(desc) {}

    VkImage image {VK_NULL_HANDLE};
    VkImageView view {VK_NULL_HANDLE};
    VkDeviceMemory memory {VK_NULL_HANDLE};
    bool bExternalMemory {false};
};
