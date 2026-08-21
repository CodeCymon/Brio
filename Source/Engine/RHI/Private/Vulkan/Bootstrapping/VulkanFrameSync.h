// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanFrameSync {
public:
    void Initialize(VulkanDevice const* inDevice);
    void Shutdown();

    void WaitOnFence() const;
    void ResetFence() const;

    [[nodiscard]] VkSemaphore GetAcquireSemaphore() const { return acquireSemaphore; }
    [[nodiscard]] VkFence Fence() const { return fence; }

private:
    VkSemaphore acquireSemaphore {};
    VkFence fence {};

    VulkanDevice const* device {nullptr};
};
