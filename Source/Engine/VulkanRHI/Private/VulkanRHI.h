// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "DynamicRHI.h"

#include "VulkanExternalTextureRegistry.h"

#include "Bootstrapping/VulkanInstance.h"
#include "Bootstrapping/VulkanSurface.h"
#include "Bootstrapping/VulkanDevice.h"
#include "Bootstrapping/VulkanSwapchain.h"
#include "Bootstrapping/VulkanTimeline.h"
#include "Bootstrapping/VulkanFrameSync.h"
#include "Bootstrapping/VulkanFrameCmdData.h"

#include "VulkanCommandList.h"
#include "Containers/StaticArray.h"


class VulkanRHI final : public IDynamicRHI, public IVulkanExternalTextureRegistry {
public:
    VulkanRHI() : device(timeline) {}

    void Initialize(NativeWindowData const &windowData, UIntPoint const& initialExtent) override;

    void Shutdown() override;

    void WaitForIdle() override;

    void OnResize(u32 width, u32 height) override;

    RHIFrameContext BeginFrame() override;
    void EndFrame() override;

    void ImmediateSubmit(Function<void(ICommandList &)> const &fn) override;

    RHITextureRef CreateTexture(RHITextureDesc const &desc, char const* debugName) override;

    RHIBufferRef CreateBuffer(RHIBufferDesc const &desc, char const* debugName) override;
    RHIMappedBufferRef CreateMappedBuffer(RHIBufferDesc const& desc, char const* debugName) override;

    // ~ IVulkanExternalTextureRegistry Begin
    VulkanTexture* RegisterExternalTexture(RHITextureDesc const &desc, VkImage image, VkImageView defaultView) override;
    void UnregisterExternalTexture(VulkanTexture* texture) override;
    // ~ IVulkanExternalTextureRegistry End

    RHIVertexShaderRef CreateVertexShader(RHIShaderDesc const &desc) override;

    RHIPixelShaderRef CreatePixelShader(RHIShaderDesc const &desc) override;

    RHIGraphicsPipelineRef CreateGraphicsPipeline(RHIGraphicsPipelineDesc const &desc) override;

private:
    VulkanInstance instance;
    VulkanSurface surface;
    VulkanDevice device;
    VulkanSwapchain swapchain;
    VulkanTimeline timeline;
    StaticArray<VulkanFrameSync, kMaxFramesInFlight> frameSyncs;
    StaticArray<VulkanFrameCmdData, kMaxFramesInFlight> frameCmdData;

    VulkanCommandList cmdList;
    
    u32 frameIndex {0};
};
