// Copyright (c) Simon Kirsch 2026.

#include "TextureTranslations.h"

VkImageType ImageTranslation::ToVulkanDimensionType(TextureDimension dimension) {
    switch (dimension) {
        case TextureDimension::Tex1D:
            return VK_IMAGE_TYPE_1D;
        case TextureDimension::Tex2D:
            return VK_IMAGE_TYPE_2D;
        case TextureDimension::Tex3D:
            return VK_IMAGE_TYPE_3D;
    }
    ASSERTM(false, "Unimplemented TextureDimension!");
    return VK_IMAGE_TYPE_MAX_ENUM;
}

VkImageUsageFlags ImageTranslation::ToVulkanUsageFlags(TextureUsage usage) {
    VkImageUsageFlags flags = 0;

    if (Contains(usage, TextureUsage::Sampled))
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (Contains(usage, TextureUsage::Storage))
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (Contains(usage, TextureUsage::ColorAttachment))
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (Contains(usage, TextureUsage::DepthStencil))
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (Contains(usage, TextureUsage::TransferSrc))
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (Contains(usage, TextureUsage::TransferDst))
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    return flags;
}

VkFormat ImageTranslation::ToVulkanFormat(PixelFormat format) {
    switch (format) {
        case PixelFormat::RGBA8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case PixelFormat::BGRA8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case PixelFormat::RGBA8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::BGRA8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::RGBA16_FLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case PixelFormat::RGBA32_FLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case PixelFormat::D32_FLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case PixelFormat::D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat::Unknown:
            return VK_FORMAT_UNDEFINED;
    }
    ASSERTM(false, "Unimplemented PixelFormat!");
    return VK_FORMAT_UNDEFINED;
}

VkSampleCountFlagBits ImageTranslation::ToVulkanSampleCount(u32 sampleCount) {
    switch (sampleCount) {
        case 1:
            return VK_SAMPLE_COUNT_1_BIT;
        case 2:
            return VK_SAMPLE_COUNT_2_BIT;
        case 4:
            return VK_SAMPLE_COUNT_4_BIT;
        case 8:
            return VK_SAMPLE_COUNT_8_BIT;
        case 16:
            return VK_SAMPLE_COUNT_16_BIT;
        case 32:
            return VK_SAMPLE_COUNT_32_BIT;
        case 64:
            return VK_SAMPLE_COUNT_64_BIT;
        default: break;
    }
    ASSERTM(false, "Unsupported Image SampleCount!");
    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

VkImageCreateInfo ImageTranslation::CreateInfoFromTextureDesc(RHITextureDesc const &desc) {
    VkImageCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = ToVulkanDimensionType(desc.dimension),
        .format = ToVulkanFormat(desc.format),
        .extent = {desc.extent.width, desc.extent.height, desc.extent.depth},
        .mipLevels = desc.mipLevels,
        .arrayLayers = desc.arrayLayers,
        .samples = ToVulkanSampleCount(desc.sampleCount),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = ToVulkanUsageFlags(desc.usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    return createInfo;
}
