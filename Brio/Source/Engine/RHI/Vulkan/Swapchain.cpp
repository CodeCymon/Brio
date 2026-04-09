#include "Swapchain.h"

#include "Device.h"
#include "VulkanCheck.h"
#include "Core/Asserts/Assert.h"

void VulkanSwapchain::init(Config const &config) {
    ASSERT(config.device, "Swapchain needs a valid device!");
    device_ = config.device;

    initPersistentData(device_->physicalDevice(), device_->surface());
    createSwapchain(config.width, config.height);
    LOG_INFO(LogRHI, "Swapchain initialized.");
}

void VulkanSwapchain::shutdown() {
    destroySwapchain();
    LOG_INFO(LogRHI, "Swapchain shutdown.");
}

// TODO: high code-duplication across create/destroy/resize -> evaluate possible refactor
void VulkanSwapchain::resize(u32 width, u32 height) {
    findCapabilities(device_->physicalDevice(), device_->surface());
    VkExtent2D extent = chooseExtent2D(surface_capabilities_, {width, height});

    VkSwapchainKHR old_swapchain = swapchain_;

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = device_->surface(),
        .minImageCount = image_count_,
        .imageFormat = surface_format_.format,
        .imageColorSpace = surface_format_.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT  | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .preTransform = surface_capabilities_.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode_,
        .clipped = VK_TRUE,
        .oldSwapchain = old_swapchain,
    };

    // TODO: improve the way this is handled
    std::array<u32, 2> indices = {
        device_->graphicsFamily(),
        device_->presentFamily()
    };
    if (indices[0] == indices[1]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<u32>(indices.size());
        createInfo.pQueueFamilyIndices = indices.data();
    }

    VkResult result = vkCreateSwapchainKHR(device_->device(), &createInfo, nullptr, &swapchain_);
    VK_CHECK(result, "Failed to create new swapchain!");

    // deleting old_swapchain and resources
    {
        for (auto view : views_) {
            vkDestroyImageView(device_->device(), view, nullptr);
        }
        views_.clear();

        for (auto semaphore : submit_semaphores_) {
            vkDestroySemaphore(device_->device(), semaphore, nullptr);
        }
        submit_semaphores_.clear();

        vkDestroySwapchainKHR(device_->device(), old_swapchain, nullptr);
    }

    vkGetSwapchainImagesKHR(device_->device(), swapchain_, &image_count_, nullptr);
    images_.resize(image_count_);
    vkGetSwapchainImagesKHR(device_->device(), swapchain_, &image_count_, images_.data());

    submit_semaphores_.resize(image_count_);
    views_.resize(image_count_);
    for (u32 i = 0; i < image_count_; i++) {
        VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images_[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surface_format_.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1
            }
        };

        vkCreateImageView(device_->device(), &viewInfo, nullptr, &views_[i]);

        VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(device_->device(), &semaphoreInfo, nullptr, &submit_semaphores_[i]);
    }
}

VkResult VulkanSwapchain::acquireNextImage(VkSemaphore acquireSemaphore) {
    return vkAcquireNextImageKHR(device_->device(), swapchain_, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, &image_index_);
}

VkResult VulkanSwapchain::present() {
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &submit_semaphores_[image_index_],
        .swapchainCount = 1,
        .pSwapchains = &swapchain_,
        .pImageIndices = &image_index_
    };

    return vkQueuePresentKHR(device_->presentQueue(), &presentInfo);
}

void VulkanSwapchain::createSwapchain(u32 width, u32 height) {
    findCapabilities(device_->physicalDevice(), device_->surface());
    VkExtent2D extent = chooseExtent2D(surface_capabilities_, {width, height});

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = device_->surface(),
        .minImageCount = image_count_,
        .imageFormat = surface_format_.format,
        .imageColorSpace = surface_format_.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .preTransform = surface_capabilities_.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode_,
        .clipped = VK_TRUE,
        .oldSwapchain = nullptr
    };

    // TODO: improve the way this is handled
    std::array<u32, 2> indices = {
        device_->graphicsFamily(),
        device_->presentFamily()
    };
    if (indices[0] == indices[1]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<u32>(indices.size());
        createInfo.pQueueFamilyIndices = indices.data();
    }

    VkResult result = vkCreateSwapchainKHR(device_->device(), &createInfo, nullptr, &swapchain_);
    VK_CHECK(result, "Failed to create swapchain!");

    vkGetSwapchainImagesKHR(device_->device(), swapchain_, &image_count_, nullptr);
    images_.resize(image_count_);
    LOG_INFO(LogRHI, "Swapchain image count: {}", image_count_);
    vkGetSwapchainImagesKHR(device_->device(), swapchain_, &image_count_, images_.data());

    submit_semaphores_.resize(image_count_);
    views_.resize(image_count_);
    for (u32 i = 0; i < image_count_; i++) {
        VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images_[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surface_format_.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1
            }
        };

        vkCreateImageView(device_->device(), &viewInfo, nullptr, &views_[i]);

        VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(device_->device(), &semaphoreInfo, nullptr, &submit_semaphores_[i]);
    }

    LOG_DETAIL(LogRHI, "Swapchain created.");
}

void VulkanSwapchain::destroySwapchain() {
    for (auto view : views_) {
        vkDestroyImageView(device_->device(), view, nullptr);
    }
    views_.clear();

    for (auto semaphore : submit_semaphores_) {
        vkDestroySemaphore(device_->device(), semaphore, nullptr);
    }
    submit_semaphores_.clear();

    vkDestroySwapchainKHR(device_->device(), swapchain_, nullptr);
    swapchain_ = nullptr;
    LOG_DETAIL(LogRHI, "Swapchain destroyed.");
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSurfaceFormat(TArray<VkSurfaceFormatKHR> const &formats) {
    ASSERT(!formats.empty(), "Format list cannot be empty!");

    constexpr std::array<VkSurfaceFormatKHR, 4> preferred_formats = {
        VkSurfaceFormatKHR{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkSurfaceFormatKHR{VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkSurfaceFormatKHR{VK_FORMAT_B8G8R8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
    };

    for (auto const& preferred : preferred_formats) {
        for (auto const& format : formats) {
            if (format.format == preferred.format && format.colorSpace == preferred.colorSpace) {
                return preferred;
            }
        }
    }

    LOG_WARN(LogRHI, "No preferred swapchain format supported! Falling back to first available format.");
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::choosePresentMode(TArray<VkPresentModeKHR> const &modes) {
    ASSERT(!modes.empty(), "Mode list cannot be empty!");

    constexpr std::array<VkPresentModeKHR, 3> preferred_modes = {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
        VK_PRESENT_MODE_FIFO_RELAXED_KHR
    };

    for (auto const& preferred : preferred_modes) {
        for (auto const& mode : modes) {
            if (preferred == mode) {
                return preferred;
            }
        }
    }

    LOG_WARN(LogRHI, "No preferred present mode supported! Falling back to VK_PRESENT_MODE_FIFO_KHR");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::chooseExtent2D(VkSurfaceCapabilitiesKHR const &capabilities, VkExtent2D const requested_extent) {
    if (capabilities.currentExtent.width != 0xFFFFFFFF) {
        return capabilities.currentExtent;
    }

    VkExtent2D extent {};
    extent.width = std::clamp(
        requested_extent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );
    extent.height = std::clamp(
        requested_extent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return extent;
}

void VulkanSwapchain::initPersistentData(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities_);

    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
    TArray<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());
    surface_format_ = chooseSurfaceFormat(formats);
    LOG_INFO(LogRHI, "Swapchain format: {}", string_VkFormat(surface_format_.format));

    u32 present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
    TArray<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());
    present_mode_ = choosePresentMode(present_modes);
    LOG_INFO(LogRHI, "Swapchain mode: {}", string_VkPresentModeKHR(present_mode_));

    image_count_ = surface_capabilities_.minImageCount + 1;
    if (surface_capabilities_.maxImageCount > 0 && image_count_ > surface_capabilities_.maxImageCount) {
        image_count_ = surface_capabilities_.maxImageCount;
    }
}

void VulkanSwapchain::findCapabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities_);
}
