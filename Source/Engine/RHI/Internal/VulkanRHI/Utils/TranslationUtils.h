#pragma once
#include "RHIResources.h"

#include "vulkan/vulkan_core.h"

namespace TranslationUtils {
    VkCullModeFlags CullMode(ECullMode cullMode);
    VkFormat PixelFormat(EPixelFormat format);
}
