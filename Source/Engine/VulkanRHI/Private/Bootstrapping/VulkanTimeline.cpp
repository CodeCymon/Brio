// Copyright (c) Simon Kirsch 2026.

#include "VulkanTimeline.h"

#include "VulkanDevice.h"
#include "VulkanCheck.h"

void VulkanTimeline::Initialize(VulkanDevice const* inDevice) {
    device = inDevice;

    VkSemaphoreTypeCreateInfo typeInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = 0,
    };
    VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &typeInfo,
    };
    VkResult result = vkCreateSemaphore(device->LogicalDevice(), &createInfo, nullptr, &semaphore);
    VK_CHECK(result);
}

void VulkanTimeline::Shutdown() {
    vkDestroySemaphore(device->LogicalDevice(), semaphore, nullptr);
    semaphore = nullptr;
}

void VulkanTimeline::WaitUntil(u64 value) const {
    if (value == 0) return;

    VkSemaphoreWaitInfo waitInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &semaphore,
        .pValues = &value
    };
    vkWaitSemaphores(device->LogicalDevice(), &waitInfo, UINT64_MAX);
}
