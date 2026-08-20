// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "RHIAPI.h"
#include "CoreMinimal.h"

struct NativeWindowData;

constexpr u32 kMaxFramesInFlight = 2;

class RHI_API IDynamicRHI {
public:
    virtual ~IDynamicRHI() = default;

    virtual void Initialize(NativeWindowData const& windowData) = 0;
    virtual void Shutdown() = 0;

    virtual void WaitForIdle() = 0;

    virtual void OnResize(u32 width, u32 height) = 0;

    // virtual RHIFrameContext BeginFrame() = 0;
    // virtual void EndFrame() = 0;


    // virtual RHIShaderRef CreateShader(ShaderDesc const& desc, char const* debugName) = 0;
    // virtual void DestroyShader(RHIShaderRef shader) = 0;

    // virtual RHIPipelineRef CreateGraphicsPipeline(GraphicsPipelineDesc const& desc, char const* debugName) = 0;
    // virtual RHIPipelineRef CreateComputePipeline(ComputePipelineDesc const& desc, char const* debugName) = 0;
    // virtual void DestroyPipeline(RHIPipelineRef pipeline) = 0;
};
