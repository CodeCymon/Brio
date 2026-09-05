// Copyright (c) Simon Kirsch 2026.

#include "BufferTranslations.h"

#include "RHIResources.h"

VkBufferUsageFlags BufferTranslation::ToVulkanUsageFlags(BufferUsage usage) {
    VkBufferUsageFlags flags = 0;
    if (Contains(usage, BufferUsage::TransferSrc))
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if (Contains(usage, BufferUsage::TransferDst))
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (Contains(usage, BufferUsage::StorageBuffer))
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (Contains(usage, BufferUsage::UniformBuffer))
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (Contains(usage, BufferUsage::IndexBuffer))
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (Contains(usage, BufferUsage::ShaderAddressable))
        flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    return flags;
}

VkBufferCreateInfo BufferTranslation::CreateInfoFromDesc(RHIBufferDesc const &desc) {
    VkBufferCreateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = desc.size,
        .usage = ToVulkanUsageFlags(desc.usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    return bufferInfo;
}
