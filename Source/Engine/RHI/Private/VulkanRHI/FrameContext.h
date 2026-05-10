#pragma once
#include <vulkan/vulkan_core.h>


class VulkanDevice;


class VulkanFrameContext {
public:
    void Initialize(VulkanDevice const* deviceRef);
    void Shutdown();

    void WaitForFence() const;
    void ResetFence() const;

    [[nodiscard]] VkCommandBuffer Cmd() const { return commandBuffer; }
    [[nodiscard]] VkFence Fence() const { return fence; }
    [[nodiscard]] VkSemaphore GetAcquireSemaphore() const { return acquireSemaphore; }

private:
    void CreateCommandPool();
    void CreateCommandBuffer();
    void CreateSyncObjects();

    void DestroyCommandPool();
    void DestroyCommandBuffer();
    void DestroySyncObjects();

private:
    VkCommandPool commandPool {};
    VkCommandBuffer commandBuffer {};

    /// @see https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    VkSemaphore acquireSemaphore {};
    VkFence fence {};

    VulkanDevice const* device {nullptr};
};
