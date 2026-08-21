// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "DynamicRHI.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanSurface.h"
#include "Vulkan/VulkanSwapchain.h"


class VulkanRHI final : public IDynamicRHI {
public:
    void Initialize(NativeWindowData const &windowData) override;

    void Shutdown() override;

    void WaitForIdle() override;

    void OnResize(u32 width, u32 height) override;

    // RHIFrameContext BeginFrame() override;
    // void EndFrame() override;

private:
    VulkanInstance instance;
    VulkanSurface surface;
    VulkanDevice device;
    VulkanSwapchain swapchain;
    // StaticArray<VulkanFrameContext, kMaxFramesInFlight> frames;

    u32 frameIndex {0};
};
