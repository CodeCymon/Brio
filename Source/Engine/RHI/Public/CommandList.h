// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "RHIResources.h"

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

    virtual void BeginRendering(RHITexture* colorTarget) = 0;
    virtual void EndRendering() = 0;
};
