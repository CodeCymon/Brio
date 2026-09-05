// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan.h>

#include "Core/CoreTypes.h"
#include "Containers/Array.h"

#include "Resources/VulkanResources.h"

class IVulkanExternalTextureRegistry;
class VulkanSurface;
class VulkanDevice;

class VulkanSwapchain {
public:
    VulkanSwapchain() = default;
    ~VulkanSwapchain() = default;

    NON_COPYABLE(VulkanSwapchain);
    NON_MOVEABLE(VulkanSwapchain);

    void Initialize(VulkanDevice const* inDevice, VulkanSurface const* inSurface, IVulkanExternalTextureRegistry* registry, UIntPoint const& inExtent);
    void Shutdown();

    void Resize(UIntPoint const& newExtent);

    VkResult AcquireNextImage(VkSemaphore acquireSemaphore);
    VkResult Present();

    [[nodiscard]] VkSwapchainKHR Swapchain() const { return swapchain; }
    [[nodiscard]] VkFormat Format() const { return surfaceFormat.format; }
    [[nodiscard]] VkExtent2D Extent() const { return extent; }
    [[nodiscard]] VkImage Image() const { return images[imageIndex]; }
    [[nodiscard]] VulkanTexture* CurrentTexture() const { return textures[imageIndex]; }
    [[nodiscard]] VkSemaphore GetSubmitSemaphore() const { return submitSemaphores[imageIndex]; }

private:
    void CreateSwapchain(UIntPoint const& inExtent, VkSwapchainKHR oldSwapchain = nullptr);
    void CreateSwapchainResources();

    void DestroySwapchain(VkSwapchainKHR& swapchainToDestroy);
    void DestroySwapchainResources();

    void InitializePersistentData();
    void UpdateCapabilities();

    void RebuildTextures();

    static VkSurfaceFormatKHR ChooseSurfaceFormat(Array<VkSurfaceFormatKHR> const& formats);
    static VkPresentModeKHR ChoosePresentMode(Array<VkPresentModeKHR> const& modes);
    static VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR const& capabilities, VkExtent2D const& requestedExtent);

private:
    VkSwapchainKHR swapchain {};
    u32 imageIndex {0};

    Array<VkImage> images {};
    Array<VkImageView> views {};
    VkExtent2D extent {};

    Array<VulkanTexture*> textures {};

    /// @see https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    Array<VkSemaphore> submitSemaphores {};

    VkSurfaceCapabilitiesKHR surfaceCapabilities {};
    VkSurfaceFormatKHR surfaceFormat {};
    VkPresentModeKHR presentMode {};
    u32 imageCount {};

    VulkanDevice const* device {nullptr};
    VulkanSurface const* surface {nullptr};
    IVulkanExternalTextureRegistry* textureRegistry {nullptr};
};
