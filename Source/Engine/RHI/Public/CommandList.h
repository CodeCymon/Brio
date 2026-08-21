// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "RHIAPI.h"

class RHI_API ICommandList {
public:
    virtual ~ICommandList() = default;

    virtual void BeginDebugLabel(const char* label) = 0;
    virtual void EndDebugLabel() = 0;
    virtual void InsertDebugLabel(const char* label) = 0;

    // virtual void ClearImage(RHITextureRef texture, ClearColor clearColor) = 0;

    // virtual void BeginRendering(RHITextureRef target) = 0;
    // virtual void EndRendering() = 0;

    // virtual void BindPipeline(RHIPipelineRef graphicsPipeline) = 0;

    // virtual void SetViewport(f32 x, f32 y, f32 width, f32 height) = 0;
    // virtual void SetScissor(i32 x, i32 y, f32 width, f32 height) = 0;

    // virtual void Draw(u32 vertexCount, u32 startIndex) = 0;
};
