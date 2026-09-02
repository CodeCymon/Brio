// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>

#include "RHIResources.h"

class VulkanTexture;

class IVulkanExternalTextureRegistry {
public:
    virtual ~IVulkanExternalTextureRegistry() = default;
    virtual VulkanTexture* RegisterExternalTexture(RHITextureDesc const& desc, VkImage image) = 0;
    virtual void UnregisterExternalTexture(VulkanTexture* texture) = 0;
};
