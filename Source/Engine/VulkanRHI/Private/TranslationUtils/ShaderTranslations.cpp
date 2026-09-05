// Copyright (c) Simon Kirsch 2026.

#include "ShaderTranslations.h"

#include "RHIDefinitions.h"

VkShaderStageFlags ShaderTranslations::ToVulkanShaderStages(ShaderStage stages) {
    VkShaderStageFlags flags = 0;
    if (Contains(stages, ShaderStage::Vertex))
        flags |= VK_SHADER_STAGE_VERTEX_BIT;
    if (Contains(stages, ShaderStage::Pixel))
        flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if (Contains(stages, ShaderStage::Compute))
        flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    return flags;
}
