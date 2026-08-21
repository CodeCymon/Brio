// Copyright (c) Simon Kirsch 2026.

#include "VulkanSwapchain.h"

#include "LogRHI.h"
#include "Vulkan/VulkanCheck.h"
#include "Vulkan/Bootstrapping/VulkanDevice.h"
#include "Vulkan/Bootstrapping/VulkanSurface.h"

void VulkanSwapchain::Initialize(VulkanDevice const* inDevice, VulkanSurface const* inSurface, UIntPoint const &inExtent) {
    device = inDevice;
    surface = inSurface;

    InitializePersistentData();

    CreateSwapchain(inExtent);
    CreateSwapchainResources();
}

void VulkanSwapchain::Shutdown() {
    DestroySwapchainResources();
    DestroySwapchain(swapchain);
}

void VulkanSwapchain::Resize(UIntPoint const &newExtent) {
    VkSwapchainKHR oldSwapchain = swapchain;
    UpdateCapabilities();
    CreateSwapchain(newExtent, oldSwapchain);
    DestroySwapchainResources();
    DestroySwapchain(oldSwapchain);
    CreateSwapchainResources();
}

VkResult VulkanSwapchain::AcquireNextImage(VkSemaphore acquireSemaphore) {
    return vkAcquireNextImageKHR(device->LogicalDevice(), swapchain, UINT64_MAX,
                                 acquireSemaphore,nullptr, &imageIndex);
}

VkResult VulkanSwapchain::Present() {
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &submitSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &imageIndex,
    };

    return vkQueuePresentKHR(device->PresentQueue(), &presentInfo);
}

void VulkanSwapchain::CreateSwapchain(UIntPoint const &inExtent, VkSwapchainKHR oldSwapchain) {
    extent = ChooseExtent(surfaceCapabilities, {inExtent.x, inExtent.y});

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->Surface(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = oldSwapchain
    };

    Set<u32> indices;
    indices.Add(device->GraphicsQueueFamilyIndex());
    indices.Add(device->PresentQueueFamilyIndex());
    ASSERTM(indices.Size() == 1, "Concurrent Image sharing is not implemented!");

    VkResult result = vkCreateSwapchainKHR(device->LogicalDevice(), &createInfo, nullptr, &swapchain);
    VK_CHECK(result);
}

void VulkanSwapchain::CreateSwapchainResources() {
    vkGetSwapchainImagesKHR(device->LogicalDevice(), swapchain, &imageCount, nullptr);

    images.Resize(imageCount);
    vkGetSwapchainImagesKHR(device->LogicalDevice(), swapchain, &imageCount, images.Data());

    submitSemaphores.Resize(imageCount);
    views.Resize(imageCount);
    for (u32 i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surfaceFormat.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1
            }
        };


        vkCreateImageView(device->LogicalDevice(), &viewInfo, nullptr, &views[i]);

        VkSemaphoreCreateInfo semaphoreInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(device->LogicalDevice(), &semaphoreInfo, nullptr, &submitSemaphores[i]);
    }
}

void VulkanSwapchain::DestroySwapchain(VkSwapchainKHR& swapchainToDestroy) {
    vkDestroySwapchainKHR(device->LogicalDevice(), swapchainToDestroy, nullptr);
    swapchainToDestroy = nullptr;
}

void VulkanSwapchain::DestroySwapchainResources() {
    for (auto view : views) {
        vkDestroyImageView(device->LogicalDevice(), view, nullptr);
    }

    for (auto semaphore : submitSemaphores) {
        vkDestroySemaphore(device->LogicalDevice(), semaphore, nullptr);
    }
}

void VulkanSwapchain::InitializePersistentData() {
    UpdateCapabilities();

    u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->PhysicalDevice(), surface->Surface(), &formatCount, nullptr);
    Array<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->PhysicalDevice(), surface->Surface(), &formatCount, formats.Data());
    surfaceFormat = ChooseSurfaceFormat(formats);

    u32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->PhysicalDevice(), surface->Surface(), &presentModeCount, nullptr);
    Array<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->PhysicalDevice(), surface->Surface(), &presentModeCount, presentModes.Data());
    presentMode = ChoosePresentMode(presentModes);

    imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        imageCount = surfaceCapabilities.maxImageCount;

    LOG_INFO(LogRHI, "Swapchain format: {}", string_VkFormat(surfaceFormat.format));
    LOG_INFO(LogRHI, "Swapchain mode: {}", string_VkPresentModeKHR(presentMode));
}

void VulkanSwapchain::UpdateCapabilities() {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->PhysicalDevice(), surface->Surface(), &surfaceCapabilities);
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(Array<VkSurfaceFormatKHR> const &formats) {
    constexpr StaticArray preferredFormats = {
        VkSurfaceFormatKHR{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkSurfaceFormatKHR{VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkSurfaceFormatKHR{VK_FORMAT_B8G8R8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
    };

    for (auto const& preferred : preferredFormats) {
        for (auto const& format : formats) {
            if (format.format == preferred.format && format.colorSpace == preferred.colorSpace)
                return format;
        }
    }

    LOG_WARNING(LogRHI, "None of the preferred swapchain formats are supported! Falling back to first available format.");
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(Array<VkPresentModeKHR> const &modes) {
    constexpr StaticArray preferredModes = {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
        VK_PRESENT_MODE_FIFO_RELAXED_KHR
    };

    for (auto const& preferred : preferredModes) {
        for (auto const& mode : modes) {
            if (preferred == mode)
                return preferred;
        }
    }

    LOG_WARNING(LogRHI, "None of the preferred present modes are supported! Falling back to VK_PRESENT_MODE_FIFO_KHR");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseExtent(VkSurfaceCapabilitiesKHR const &capabilities,
    VkExtent2D const &requestedExtent) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent;
    actualExtent.width = Math::Clamp(
        requestedExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );
    actualExtent.height = Math::Clamp(
        requestedExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return actualExtent;
}
