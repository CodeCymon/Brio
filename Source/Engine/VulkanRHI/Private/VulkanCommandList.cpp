// Copyright (c) Simon Kirsch 2026.

#include "VulkanCommandList.h"

#include "Resources/VulkanResources.h"
#include "TranslationUtils/TextureTranslations.h"

void VulkanCommandList::BeginDebugLabel(char const* label) { ASSERT(false); }

void VulkanCommandList::EndDebugLabel() { ASSERT(false); }

void VulkanCommandList::InsertDebugLabel(char const* label) { ASSERT(false); }

void VulkanCommandList::TransitionImage(RHITexture* texture, RHIResourceState srcState, RHIResourceState dstState) {
    auto* vkTexture = ResourceCast(texture);
    VulkanImageStateInfo src = ImageTranslation::ToVulkanImageState(srcState);
    VulkanImageStateInfo dst = ImageTranslation::ToVulkanImageState(dstState);

    VkImageMemoryBarrier2 imageBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = src.stage, .srcAccessMask = src.access,
        .dstStageMask = dst.stage, .dstAccessMask = dst.access,
        .oldLayout = src.layout, .newLayout = dst.layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = vkTexture->Image(),
        .subresourceRange = {
            .aspectMask = ImageTranslation::ToVulkanImageAspectFlags(vkTexture->Desc().format),
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

void VulkanCommandList::BlitImage(RHITexture* srcTexture, RHITexture* dstTexture) {
    VulkanTexture* src = ResourceCast(srcTexture);
    VulkanTexture* dst = ResourceCast(dstTexture);

    VkImageBlit2 blit = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        .pNext = nullptr,
        .srcSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .srcOffsets = {
            {0,0,0},
            {(i32)src->Desc().extent.width, (i32)src->Desc().extent.height, (i32)src->Desc().extent.depth},
        },
        .dstSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .dstOffsets = {
            {0,0,0},
            {(i32)dst->Desc().extent.width, (i32)dst->Desc().extent.height, (i32)dst->Desc().extent.depth},
        },
    };

    VkBlitImageInfo2 imageInfo = {
        .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .pNext = nullptr,
        .srcImage =  src->Image(),
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage =  dst->Image(),
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &blit,
        .filter = VK_FILTER_NEAREST,
    };
    vkCmdBlitImage2(cmd, &imageInfo);
}

void VulkanCommandList::BindActiveCommandBuffer(VkCommandBuffer commandBuffer) {
    cmd = commandBuffer;
}
