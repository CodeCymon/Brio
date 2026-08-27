// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "RHIAPI.h"

#include "CoreMinimal.h"
#include "RHIResources.h"

enum class RHIResourceState : u8 {
    Undefined,
    TransferSrc,
    TransferDst,
    ColorAttachment,
    DepthStencilAttachment,
    DepthStencilReadOnly,
    ShaderReadOnly,
    General,
    Present,
};

struct ClearColor {
    f32 r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

class RHI_API ICommandList {
public:
    virtual ~ICommandList() = default;

    virtual void BeginDebugLabel(const char* label) = 0;
    virtual void EndDebugLabel() = 0;
    virtual void InsertDebugLabel(const char* label) = 0;

    virtual void TransitionImage(RHITexture* texture, RHIResourceState srcState, RHIResourceState dstState) = 0;

    virtual void ClearImage(RHITexture* texture, ClearColor clearColor) = 0;

    virtual void BlitImage(RHITexture* srcTexture, RHITexture* dstTexture) = 0;
};
