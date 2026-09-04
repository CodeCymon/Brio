// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "CoreMinimal.h"

#include "RHI.h"
#include "RHIResources.h"

class ICommandList;
struct NativeWindowData;

constexpr u32 kMaxFramesInFlight = 2;

struct RHIFrameContext {
    ICommandList* cmdList {nullptr};
    RHITexture* backBuffer {};
    u32 frameIndex {0};
};


class IDynamicRHI {
public:
    virtual ~IDynamicRHI() = default;

    virtual void Initialize(NativeWindowData const& windowData, UIntPoint const& initialExtent) = 0;
    virtual void Shutdown() = 0;

    virtual void WaitForIdle() = 0;

    virtual void OnResize(u32 width, u32 height) = 0;

    virtual RHIFrameContext BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual RHITextureRef CreateTexture(RHITextureDesc const& desc, char const* debugName) = 0;

    virtual RHIVertexShaderRef CreateVertexShader(RHIShaderDesc const& desc) = 0;

    virtual RHIPixelShaderRef CreatePixelShader(RHIShaderDesc const& desc) = 0;

    virtual RHIGraphicsPipelineRef CreateGraphicsPipeline(RHIGraphicsPipelineDesc const& desc) = 0;

};

inline RHIFrameContext RHIBeginFrame() {
    return GDynamicRHI->BeginFrame();
}

inline void RHIEndFrame() {
    GDynamicRHI->EndFrame();
}

inline RHITextureRef RHICreateTexture(RHITextureDesc const &desc, char const* debugName) {
    return GDynamicRHI->CreateTexture(desc, debugName);
}

inline RHIVertexShaderRef RHICreateVertexShader(RHIShaderDesc const &desc) {
    return GDynamicRHI->CreateVertexShader(desc);
}

inline RHIPixelShaderRef RHICreatePixelShader(RHIShaderDesc const &desc) {
    return GDynamicRHI->CreatePixelShader(desc);
}

inline RHIGraphicsPipelineRef RHICreateGraphicsPipeline(RHIGraphicsPipelineDesc const& desc) {
    return GDynamicRHI->CreateGraphicsPipeline(desc);
}