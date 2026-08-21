// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

#include "LogRHI.h"

void VulkanRHI::Initialize(NativeWindowData const &windowData) {
    instance.Initialize(true);
    surface.Initialize(&instance, windowData);
    device.Initialize(&instance, &surface);
    swapchain.Initialize(&device, &surface, {800, 450});
}

void VulkanRHI::Shutdown() {
    WaitForIdle();

    swapchain.Shutdown();
    device.Shutdown();
    surface.Shutdown();
    instance.Shutdown();
}

void VulkanRHI::WaitForIdle() {
    vkDeviceWaitIdle(device.LogicalDevice());
}

void VulkanRHI::OnResize(u32 width, u32 height) {
    if (width == 0 || height == 0) return;

    WaitForIdle();
    swapchain.Resize({width, height});
    LOG_VERBOSE(LogRHI, "Resized swapchain to: {}x{}", width, height);
}
