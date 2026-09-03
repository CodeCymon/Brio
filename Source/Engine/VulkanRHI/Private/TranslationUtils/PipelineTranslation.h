// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>

#include "RHIDefinitions.h"

namespace PipelineTranslation {
    VkCullModeFlags ToVulkanCullMode(CullMode cullMode);
    VkPolygonMode ToVulkanPolygonMode(FillMode fillMode);
}