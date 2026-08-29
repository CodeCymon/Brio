// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHIAPI.h"

#include "RHI.h"
#include "VulkanRHI.h"

VULKANRHI_API IDynamicRHI* GDynamicRHI {nullptr};

VULKANRHI_API void RHI::Create(NativeWindowData const &windowData) {
    GDynamicRHI = new VulkanRHI();
    GDynamicRHI->Initialize(windowData);
}

VULKANRHI_API void RHI::Destroy() {
    GDynamicRHI->Shutdown();
    delete GDynamicRHI;
    GDynamicRHI = nullptr;
}
