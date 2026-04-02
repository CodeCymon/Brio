#pragma once
#include <vulkan/vulkan_core.h>

class VulkanFrameContext {
public:
    struct Config {
        class VulkanDevice const* device;
    };

    void init(Config const& config);
    void shutdown();

    void waitForFence() const;
    void resetFence() const;

    [[nodiscard]] VkCommandBuffer cmd() const { return command_buffer_; }
    [[nodiscard]] VkFence fence() const { return fence_; }
    [[nodiscard]] VkSemaphore acquireSemaphore() const { return acquire_semaphore_; }

private:
    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();

    void destroyCommandPool();
    void destroyCommandBuffer();
    void destroySyncObjects();

private:
    VkCommandPool command_pool_{};
    VkCommandBuffer command_buffer_{};

    VkFence fence_{};
    /* See https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
     * for info on why one set of semaphores is FRAMES_IN_FLIGHT
     * and the other semaphore set is SWAPCHAIN_IMAGE_COUNT
     * @brief: Naming convention from said document's example code.
     */
    VkSemaphore acquire_semaphore_{};

    class VulkanDevice const* device_{};
};
