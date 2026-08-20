// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

void VulkanRHI::Initialize(NativeWindowData const &windowData) {
    instance.Initialize(true);
}

void VulkanRHI::Shutdown() {
    instance.Shutdown();
}

void VulkanRHI::WaitForIdle() {}

void VulkanRHI::OnResize(u32 width, u32 height) {}
