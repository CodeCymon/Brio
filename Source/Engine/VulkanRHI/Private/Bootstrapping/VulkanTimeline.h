// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan.h>
#include "CoreMinimal.h"

class VulkanDevice;

class VulkanTimeline {
public:
    void Initialize(VulkanDevice const* inDevice);
    void Shutdown();

    void WaitUntil(u64 value) const;

    [[nodiscard]] u64 NextSignalValue() { return ++counter; }
    [[nodiscard]] u64 PendingSubmitValue() const { return counter + 1; }
    [[nodiscard]] VkSemaphore Semaphore() const { return semaphore; }

private:
    VkSemaphore semaphore {};
    u64 counter {0};
    VulkanDevice const* device {nullptr};
};
