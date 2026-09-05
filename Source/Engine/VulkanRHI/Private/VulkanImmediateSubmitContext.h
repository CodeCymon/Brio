// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>

class VulkanDevice;

class VulkanImmediateSubmitContext {
public:
    VulkanImmediateSubmitContext(VulkanDevice const& device);

    bool Initialize();
    void Shutdown();

    VkCommandPool Pool() const { return pool; }
    VkCommandBuffer Cmd() const { return cmd; }
    VkFence Fence() const { return fence; }

private:
    VkCommandPool pool {};
    VkCommandBuffer cmd {};
    VkFence fence {};
    VulkanDevice const* device{};
};
