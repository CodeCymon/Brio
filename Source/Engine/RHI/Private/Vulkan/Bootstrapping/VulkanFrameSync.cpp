// Copyright (c) Simon Kirsch 2026.

#include "VulkanFrameSync.h"

#include "LogRHI.h"
#include "Vulkan//VulkanCheck.h"
#include "Vulkan/Bootstrapping/VulkanDevice.h"

void VulkanFrameSync::Initialize(VulkanDevice const* inDevice) {
    device = inDevice;

    VkSemaphoreCreateInfo semaphoreInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkResult semaphoreResult = vkCreateSemaphore(device->LogicalDevice(), &semaphoreInfo, nullptr, &acquireSemaphore);
    VK_CHECK(semaphoreResult);
}

void VulkanFrameSync::Shutdown() {
    vkDestroySemaphore(device->LogicalDevice(), acquireSemaphore, nullptr);
    acquireSemaphore = nullptr;
}
