#pragma once

#include "RHIAPI.h"
#include "CoreMinimal.h"

#include "RHIResources.h"


struct FNativeWindowHandle;

constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

class RHI_API IDynamicRHI {
public:
    virtual ~IDynamicRHI() = default;

    virtual void Initialize(FNativeWindowHandle const& windowHandle) = 0;
    virtual void Shutdown() = 0;

    virtual void WaitForIdle() = 0;

    virtual void OnResize(u32 width, u32 height) = 0;

    virtual FRHIFrameContext BeginFrame() = 0;
    virtual void EndFrame() = 0;


    virtual FRHIShaderRef CreateShader(FShaderDesc const& desc) = 0;
    virtual void DestroyShader(FRHIShaderRef shader) = 0;

    virtual FRHITextureRef CreateTexture(FTextureDesc const& desc) = 0;
    virtual void DestroyTexture(FRHITextureRef texture) = 0;

    virtual FRHIGraphicsPipelineRef CreateGraphicsPipeline(FGraphicsPipelineDesc const& desc) = 0;
    virtual void DestroyGraphicsPipeline(FRHIGraphicsPipelineRef graphicsPipeline) = 0;


    virtual void BeginRendering(FRHITextureRef colorTarget) = 0;
    virtual void EndRendering(FRHITextureRef colorTarget) = 0;
    virtual void BindPipeline(FRHIGraphicsPipelineRef graphicsPipeline) = 0;

    virtual void SetViewport(f32 x, f32 y, f32 width, f32 height) = 0;
    virtual void SetScissor(i32 x, i32 y, u32 width, u32 height) = 0;

    virtual void Draw(u32 numVertices, u32 firstVertex = 0) = 0;
};

extern RHI_API IDynamicRHI* GDynamicRHI;

namespace RHI {
    RHI_API void Create(FNativeWindowHandle const& windowHandle);
    RHI_API void Destroy();

    RHI_API IDynamicRHI& Get();
}