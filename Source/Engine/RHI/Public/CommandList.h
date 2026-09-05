// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "RHIDefinitions.h"
#include "Math/Vector2.h"
#include "Math/IntPoint.h"

class RHIBuffer;
class RHIGraphicsPipeline;
class RHITexture;

class ICommandList {
public:
    virtual ~ICommandList() = default;

    virtual void BeginDebugLabel(const char* label) = 0;
    virtual void EndDebugLabel() = 0;
    virtual void InsertDebugLabel(const char* label) = 0;

    virtual void TransitionImage(RHITexture* texture, RHIResourceState srcState, RHIResourceState dstState) = 0;

    virtual void ClearImage(RHITexture* texture, ClearColor clearColor) = 0;

    virtual void BlitImage(RHITexture* srcTexture, RHITexture* dstTexture) = 0;

    virtual void BindPipeline(RHIGraphicsPipeline* pipeline) = 0;

    virtual void PushConstants(RHIGraphicsPipeline* pipeline, ShaderStage stages, u64 offset, u64 size, const void* data) = 0;

    virtual void BeginRendering(RHITexture* colorTarget) = 0;
    virtual void EndRendering() = 0;

    virtual void SetViewport(Vec2 offset, Vec2 size) = 0;
    virtual void SetScissor(IntPoint offset, UIntPoint size) = 0;

    virtual void Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) = 0;

    virtual void CopyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer, u64 size, u64 srcOffset, u64 dstOffset) = 0;
};
