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

    void BindPipeline(RHIGraphicsPipeline* pipeline) override;

    void PushConstants(RHIGraphicsPipeline* pipeline, ShaderStage stages, u64 offset, u64 size, const void* data) override;

    void BeginRendering(RHITexture* colorTarget) override;
    void EndRendering() override;

    void SetViewport(Vec2 offset, Vec2 size) override;
    void SetScissor(IntPoint offset, UIntPoint size) override;

    void Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) override;

public:
    void BindActiveCommandBuffer(VkCommandBuffer commandBuffer);

private:
    VkCommandBuffer cmd {};
};