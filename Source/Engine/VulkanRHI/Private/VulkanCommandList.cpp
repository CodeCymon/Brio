// Copyright (c) Simon Kirsch 2026.

#include "VulkanCommandList.h"

#include "Resources/VulkanResources.h"
#include "TranslationUtils/ShaderTranslations.h"
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
            {(i32)src->Desc().extent.x, (i32)src->Desc().extent.y, (i32)src->Desc().extent.z},
        },
        .dstSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .dstOffsets = {
            {0,0,0},
            {(i32)dst->Desc().extent.x, (i32)dst->Desc().extent.y, (i32)dst->Desc().extent.z},
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

void VulkanCommandList::BindPipeline(RHIGraphicsPipeline* pipeline) {
    VulkanGraphicsPipeline* vkPipeline = ResourceCast(pipeline);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->Pipeline());
}

void VulkanCommandList::PushConstants(RHIGraphicsPipeline* pipeline, ShaderStage stages, u64 offset, u64 size, const void* data) {
    VulkanGraphicsPipeline* vkPipeline = ResourceCast(pipeline);
    VkShaderStageFlags vkStages = ShaderTranslations::ToVulkanShaderStages(stages);
    vkCmdPushConstants(cmd, vkPipeline->Layout(), vkStages, offset, size, data);
}

void VulkanCommandList::BeginRendering(RHITexture* colorTarget) {
    VulkanTexture* texture = ResourceCast(colorTarget);

    VkRenderingAttachmentInfo attachmentInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = texture->DefaultView(),
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {.color = {0,0,0,1}}
    };

    VkRenderingInfo renderingInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {{0,0}, {colorTarget->Desc().extent.x, colorTarget->Desc().extent.y}},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };
    vkCmdBeginRendering(cmd, &renderingInfo);
}

void VulkanCommandList::SetViewport(Vec2 offset, Vec2 size) {
    VkViewport viewport {
        .x = offset.x, .y = offset.y,
        .width = size.x, .height = size.y,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(cmd, 0, 1, &viewport);
}

void VulkanCommandList::SetScissor(IntPoint offset, UIntPoint size) {
    VkRect2D scissor {
            {.x = offset.x, .y = offset.y,},
            {.width = size.x, .height = size.y},
        };
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void VulkanCommandList::Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) {
    vkCmdDraw(cmd, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandList::CopyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer, u64 size,
    u64 srcOffset, u64 dstOffset) {
    VulkanBuffer* src = ResourceCast(srcBuffer);
    VulkanBuffer* dst = ResourceCast(dstBuffer);

    VkBufferCopy copyRegion {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = size,
    };
    vkCmdCopyBuffer(cmd, src->Buffer(), dst->Buffer(), 1, &copyRegion);
}

void VulkanCommandList::EndRendering() {
    vkCmdEndRendering(cmd);
}

void VulkanCommandList::BindActiveCommandBuffer(VkCommandBuffer commandBuffer) {
    cmd = commandBuffer;
}
