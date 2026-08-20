// Copyright (c) Simon Kirsch 2026.

#include "RHI.h"
#include "DynamicRHI.h"

#include <memory>

#include "Vulkan/VulkanRHI.h"

IDynamicRHI* GDynamicRHI {nullptr};

namespace RHI {
    static std::unique_ptr<IDynamicRHI> sInstance {nullptr};
}

void RHI::Create(NativeWindowData const &windowData) {
    sInstance = std::make_unique<VulkanRHI>();
    GDynamicRHI = sInstance.get();
    GDynamicRHI->Initialize(windowData);
}

void RHI::Destroy() {
    GDynamicRHI->Shutdown();
    sInstance.reset();
    GDynamicRHI = nullptr;
}
