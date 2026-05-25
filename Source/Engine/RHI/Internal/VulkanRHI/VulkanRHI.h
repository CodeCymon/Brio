#pragma once

#include "CoreMinimal.h"
#include "Containers/Pool.h"

#include "DynamicRHI.h"

#include "Device.h"
#include "FrameContext.h"
#include "Swapchain.h"


class VulkanRHI final : public IDynamicRHI {
public:
    VulkanRHI();
    ~VulkanRHI() override;

    void Initialize(FNativeWindowHandle const &windowHandle) override;
    void Shutdown() override;

    void WaitForIdle() override;

    void OnResize(u32 width, u32 height) override;

    FRHIFrameContext BeginFrame() override;
    void EndFrame() override;


    FRHIShaderRef CreateShader(FShaderDesc const &desc) override;
    void DestroyShader(FRHIShaderRef shader) override;

    FRHITextureRef CreateTexture(FTextureDesc const &desc) override;
    void DestroyTexture(FRHITextureRef texture) override;

    FRHIGraphicsPipelineRef CreateGraphicsPipeline(FGraphicsPipelineDesc const &desc) override;
    void DestroyGraphicsPipeline(FRHIGraphicsPipelineRef graphicsPipeline) override;


    void BeginRendering(FRHITextureRef colorTarget) override;
    void EndRendering(FRHITextureRef colorTarget) override;

    void BindPipeline(FRHIGraphicsPipelineRef graphicsPipeline) override;

    void SetViewport(f32 x, f32 y, f32 width, f32 height) override;
    void SetScissor(i32 x, i32 y, u32 width, u32 height) override;

    void Draw(u32 numVertices,u32 firstVertex) override;

private:
    VulkanDevice device;
    VulkanSwapchain swapchain;
    TFixedArray<VulkanFrameContext, MAX_FRAMES_IN_FLIGHT> frames;

    u32 frameIndex {0};

    TPool<FVulkanShader> shaderPool;
    TPool<FVulkanTexture> texturePool;
    TPool<FVulkanGraphicsPipeline> pipelinePool;
};
