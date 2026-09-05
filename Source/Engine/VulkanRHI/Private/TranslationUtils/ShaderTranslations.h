// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>

#include "Core/CoreTypes.h"

enum class ShaderStage : u8;

namespace ShaderTranslations {
    VkShaderStageFlags ToVulkanShaderStages(ShaderStage stages);
}