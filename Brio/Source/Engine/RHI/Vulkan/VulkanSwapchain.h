#pragma once
#include <vulkan/vulkan_core.h>

#include "Common/Types.h"
#include "Core/Containers/Array.h"

class VulkanSwapchain {
public:
    struct Config {
        class VulkanDevice const* device;
        u32 width;
        u32 height;
    };

    VulkanSwapchain() = default;
    ~VulkanSwapchain() = default;

    VulkanSwapchain(VulkanSwapchain const&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain const&) = delete;

    void init(Config const& config);
    void shutdown();

    void resize(u32 width, u32 height);

    VkResult acquireNextImage(VkSemaphore image_available);
    VkResult present();

    [[nodiscard]] VkSwapchainKHR swapchain() const { return swapchain_; }
    [[nodiscard]] VkFormat format() const { return surface_format_.format; }
    [[nodiscard]] VkExtent2D extent() const { return extent_; }
    [[nodiscard]] VkImage image() const { return images_[image_index_]; }
    [[nodiscard]] VkImageView view() const { return views_[image_index_]; }
    [[nodiscard]] VkSemaphore semaphore() const { return render_finished_semaphores_[image_index_]; }

private:
    void createSwapchain(u32 width, u32 height);
    void destroySwapchain();

    static VkSurfaceFormatKHR chooseSurfaceFormat(TArray<VkSurfaceFormatKHR> const& formats);
    static VkPresentModeKHR choosePresentMode(TArray<VkPresentModeKHR> const& modes);
    static VkExtent2D chooseExtent2D(VkSurfaceCapabilitiesKHR const& capabilities, VkExtent2D requested_extent);
    void initPersistentData(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
    void findCapabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

private:
    VkSwapchainKHR swapchain_{};
    u32 image_index_ { 0 };
    TArray<VkImage> images_{};
    TArray<VkImageView> views_{};
    TArray<VkSemaphore> render_finished_semaphores_{};
    VkExtent2D extent_{};

    VkSurfaceCapabilitiesKHR surface_capabilities_{};
    VkSurfaceFormatKHR surface_format_{};
    VkPresentModeKHR present_mode_{};
    u32 image_count_{};

    class VulkanDevice const* device_{};
};
