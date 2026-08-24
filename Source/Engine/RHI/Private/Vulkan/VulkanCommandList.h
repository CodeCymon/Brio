// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>

#include "CommandList.h"

class VulkanCommandList : public ICommandList {
public:
    void BeginDebugLabel(char const* label) override;

    void EndDebugLabel() override;

    void InsertDebugLabel(char const* label) override;

    void ClearImage(RHITexture* texture, ClearColor clearColor) override;

public:
    void BindActiveCommandBuffer(VkCommandBuffer commandBuffer);

    void TransitionImage(RHITexture* texture, RHIResourceState srcState, RHIResourceState dstState) override;

private:
    VkCommandBuffer cmd {};
};