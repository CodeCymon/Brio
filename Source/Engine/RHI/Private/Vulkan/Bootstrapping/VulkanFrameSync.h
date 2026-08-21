// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan.h>

#include "CoreMinimal.h"

class VulkanDevice;

class VulkanFrameSync {
public:
    void Initialize(VulkanDevice const* inDevice);
    void Shutdown();

    [[nodiscard]] VkSemaphore GetAcquireSemaphore() const { return acquireSemaphore; }

    u64 timelineWaitValue {0};

private:
    /// @see https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    VkSemaphore acquireSemaphore {};

    VulkanDevice const* device {nullptr};
};
