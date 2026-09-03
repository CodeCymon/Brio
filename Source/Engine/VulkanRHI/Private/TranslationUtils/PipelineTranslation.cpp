// Copyright (c) Simon Kirsch 2026.

#include "PipelineTranslation.h"

#include "Log/Assert.h"

VkCullModeFlags PipelineTranslation::ToVulkanCullMode(CullMode cullMode) {
    switch (cullMode) {
        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
        case CullMode::None:
            return VK_CULL_MODE_NONE;
        default:
            ASSERT(false);
    }
    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
}

VkPolygonMode PipelineTranslation::ToVulkanPolygonMode(FillMode fillMode) {
    switch (fillMode) {
        case FillMode::Solid:
            return VK_POLYGON_MODE_FILL;
        case FillMode::Wireframe:
            return VK_POLYGON_MODE_LINE;
        default:
            ASSERT(false);
    }
    return VK_POLYGON_MODE_MAX_ENUM;
}
