// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

void VulkanRHI::Initialize(NativeWindowData const &windowData) {
    instance.Initialize(true);
    surface.Initialize(&instance, windowData);
    device.Initialize(&instance, &surface);
}

void VulkanRHI::Shutdown() {
    device.Shutdown();
    surface.Shutdown();
    instance.Shutdown();
}

void VulkanRHI::WaitForIdle() {}

void VulkanRHI::OnResize(u32 width, u32 height) {}
