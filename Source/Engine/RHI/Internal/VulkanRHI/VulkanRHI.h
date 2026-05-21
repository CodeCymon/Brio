#pragma once

#include "CoreMinimal.h"

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

    void OnResize(u32 width, u32 height) override;

    FRHIFrameContext BeginFrame() override;
    void EndFrame() override;

private:
    VulkanDevice device;
    VulkanSwapchain swapchain;
    TFixedArray<VulkanFrameContext, MAX_FRAMES_IN_FLIGHT> frames;

    u32 frameIndex {0};
};
