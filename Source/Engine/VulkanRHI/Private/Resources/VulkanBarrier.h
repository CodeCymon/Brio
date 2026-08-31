// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>
#include "CommandList.h"

struct VulkanImageStateInfo {
    VkImageLayout layout;
    VkPipelineStageFlags2 stage;
    VkAccessFlags2 access;
};

[[nodiscard]] VulkanImageStateInfo ToVulkanImageState(RHIResourceState state);
