// Copyright (c) Simon Kirsch 2026.

#include "VulkanCommandList.h"

void VulkanCommandList::BeginDebugLabel(char const* label) {}

void VulkanCommandList::EndDebugLabel() {}

void VulkanCommandList::InsertDebugLabel(char const* label) {}

void VulkanCommandList::ClearImage(RHITexture* texture, ClearColor clearColor) {}

void VulkanCommandList::BindActiveCommandBuffer(VkCommandBuffer commandBuffer) {
    cmd = commandBuffer;
}
