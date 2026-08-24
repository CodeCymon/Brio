// Copyright (c) Simon Kirsch 2026.

#include "VulkanCommandList.h"

#include "Vulkan/Resources/VulkanBarrier.h"
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

void VulkanCommandList::TransitionImage(RHITexture* texture, RHIResourceState srcState, RHIResourceState dstState) {
    auto* vkTexture = ResourceCast(texture);
    VulkanImageStateInfo src = ToVulkanImageState(srcState);
    VulkanImageStateInfo dst = ToVulkanImageState(dstState);

    VkImageMemoryBarrier2 imageBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = src.stage, .srcAccessMask = src.access,
        .dstStageMask = dst.stage, .dstAccessMask = dst.access,
        .oldLayout = src.layout, .newLayout = dst.layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = vkTexture->Image(),
        .subresourceRange = {
            .aspectMask = AspectMaskFromFormat(vkTexture->Desc().format),
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .layerCount = VK_REMAINING_ARRAY_LAYERS,
        }
    };
    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &imageBarrier,
    };
    vkCmdPipelineBarrier2(cmd, &dependencyInfo);
}
