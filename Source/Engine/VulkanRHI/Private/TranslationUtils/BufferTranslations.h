// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>

#include "Core/CoreTypes.h"

enum class BufferUsage : u32;
struct RHIBufferDesc;

namespace BufferTranslation {
    VkBufferUsageFlags ToVulkanUsageFlags(BufferUsage usage);
    VkBufferCreateInfo CreateInfoFromDesc(RHIBufferDesc const& desc);
}
