// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanFrameCmdData {
public:
    void Initialize(VulkanDevice const* inDevice);
    void Shutdown();

    void BeginCommandBuffer() const;
    void EndCommandBuffer() const;

    [[nodiscard]] VkCommandBuffer Cmd() const { return commandBuffer; }

private:
    VkCommandPool commandPool {};
    VkCommandBuffer commandBuffer {};

    VulkanDevice const* device {nullptr};
};
