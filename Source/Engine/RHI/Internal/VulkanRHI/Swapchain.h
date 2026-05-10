#pragma once

#include "CoreMinimal.h"

#include <vulkan/vulkan_core.h>

class VulkanDevice;


class VulkanSwapchain {
public:
    struct FConfig {
        VulkanDevice const* device;
        u32 width;
        u32 height;
    };

    VulkanSwapchain() = default;
    ~VulkanSwapchain() = default;

    VulkanSwapchain(VulkanSwapchain const&) = delete;
    VulkanSwapchain& operator = (VulkanSwapchain const&) = delete;
    VulkanSwapchain(VulkanSwapchain&&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

    void Initialize(FConfig const& config);
    void Shutdown();

    void Resize(u32 width, u32 height);

    VkResult AcquireNextImage(VkSemaphore acquireSemaphore);
    VkResult Present();

    [[nodiscard]] VkSwapchainKHR Swapchain() const { return swapchain; }
    [[nodiscard]] VkFormat Format() const { return surfaceFormat.format; }
    [[nodiscard]] VkExtent2D Extent() const { return extent; }
    [[nodiscard]] VkImage Image() const { return images[imageIndex]; }
    [[nodiscard]] VkImageView View() const { return views[imageIndex]; }
    [[nodiscard]] VkSemaphore GetSubmitSemaphore() const { return submitSemaphores[imageIndex]; }

private:
    void CreateSwapchain(u32 width, u32 height, VkSwapchainKHR oldSwapchain = nullptr);
    void CreateSwapchainResources();

    void DestroySwapchain(VkSwapchainKHR& swapchain);
    void DestroySwapchainResources();

    void InitializePersistentData();
    void UpdateCapabilities();

    static VkSurfaceFormatKHR ChooseSurfaceFormat(TArray<VkSurfaceFormatKHR> const& formats);
    static VkPresentModeKHR ChoosePresentMode(TArray<VkPresentModeKHR> const& modes);
    static VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR const& capabilities, VkExtent2D requestedExtent);

private:
    VkSwapchainKHR swapchain{};
    u32 imageIndex {0};

    TArray<VkImage> images{};
    TArray<VkImageView> views{};
    VkExtent2D extent{};

    /// @see https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    TArray<VkSemaphore> submitSemaphores{};

    VkSurfaceCapabilitiesKHR surfaceCapabilities{};
    VkSurfaceFormatKHR surfaceFormat{};
    VkPresentModeKHR presentMode{};
    u32 imageCount{};

    VulkanDevice const* device {nullptr};
};
