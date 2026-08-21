// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "DynamicRHI.h"

#include "Vulkan/Bootstrapping/VulkanInstance.h"
#include "Vulkan/Bootstrapping/VulkanSurface.h"
#include "Vulkan/Bootstrapping/VulkanDevice.h"
#include "Vulkan/Bootstrapping/VulkanSwapchain.h"
#include "Vulkan/Bootstrapping/VulkanFrameSync.h"
#include "Vulkan/Bootstrapping/VulkanFrameCmdData.h"

#include "Vulkan/VulkanCommandList.h"


class VulkanRHI final : public IDynamicRHI {
public:
    void Initialize(NativeWindowData const &windowData) override;

    void Shutdown() override;

    void WaitForIdle() override;

    void OnResize(u32 width, u32 height) override;

    [[nodiscard]] RHIFrameContext BeginFrame() override;
    void EndFrame() override;

private:
    VulkanInstance instance;
    VulkanSurface surface;
    VulkanDevice device;
    VulkanSwapchain swapchain;
    StaticArray<VulkanFrameSync, kMaxFramesInFlight> frameSyncs;
    StaticArray<VulkanFrameCmdData, kMaxFramesInFlight> frameCmdData;

    VulkanCommandList cmdList;

    u32 frameIndex {0};
};
