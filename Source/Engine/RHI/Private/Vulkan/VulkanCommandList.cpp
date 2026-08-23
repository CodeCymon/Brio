// Copyright (c) Simon Kirsch 2026.

#include "VulkanCommandList.h"

#include "Vulkan/Resources/VulkanResources.h"

void VulkanCommandList::BeginDebugLabel(char const* label) {}

void VulkanCommandList::EndDebugLabel() {}

void VulkanCommandList::InsertDebugLabel(char const* label) {}

void VulkanCommandList::ClearImage(RHITexture* texture, ClearColor clearColor) {
    VulkanTexture* vkTexture = ResourceCast(texture);

    VkClearColorValue color = {.float32 = {clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
    VkImageSubresourceRange range = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };
    vkCmdClearColorImage(cmd, vkTexture->Image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);
}

void VulkanCommandList::BindActiveCommandBuffer(VkCommandBuffer commandBuffer) {
    cmd = commandBuffer;
}
