// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "CoreMinimal.h"

#include "RHIResources.h"

struct NativeWindowData;

constexpr u32 kMaxFramesInFlight = 2;

struct RHIFrameContext {
    class ICommandList* cmdList {nullptr};
    RHITexture* swapchainTexture {};
    u32 frameIndex {0};
};


class IDynamicRHI {
public:
    virtual ~IDynamicRHI() = default;

    virtual void Initialize(NativeWindowData const& windowData) = 0;
    virtual void Shutdown() = 0;

    virtual void WaitForIdle() = 0;

    virtual void OnResize(u32 width, u32 height) = 0;

    [[nodiscard]] virtual RHIFrameContext BeginFrame() = 0;
    virtual void EndFrame() = 0;

    [[nodiscard]] virtual RHITextureRef CreateTexture(RHITextureDesc const& desc, char const* debugName) = 0;
};
