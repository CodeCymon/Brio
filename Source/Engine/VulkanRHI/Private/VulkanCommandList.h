// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>

#include "CommandList.h"

class VulkanCommandList final : public ICommandList {
public:
    void BeginDebugLabel(char const* label) override;

    void EndDebugLabel() override;

    void InsertDebugLabel(char const* label) override;

    void TransitionImage(RHITexture* texture, RHIResourceState srcState, RHIResourceState dstState) override;

    void ClearImage(RHITexture* texture, ClearColor clearColor) override;

    void BlitImage(RHITexture* srcTexture, RHITexture* dstTexture) override;

public:
    void BindActiveCommandBuffer(VkCommandBuffer commandBuffer);

private:
    VkCommandBuffer cmd {};
};