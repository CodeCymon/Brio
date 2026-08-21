// Copyright (c) Simon Kirsch 2026.

#include "VulkanFrameSync.h"

#include "LogRHI.h"
#include "Vulkan//VulkanCheck.h"
#include "Vulkan/Bootstrapping/VulkanDevice.h"

void VulkanFrameSync::Initialize(VulkanDevice const* inDevice) {
    device = inDevice;

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    VkResult fenceResult = vkCreateFence(device->LogicalDevice(), &fenceInfo, nullptr, &fence);
    VK_CHECK(fenceResult);

    VkSemaphoreCreateInfo semaphoreInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkResult semaphoreResult = vkCreateSemaphore(device->LogicalDevice(), &semaphoreInfo, nullptr, &acquireSemaphore);
    VK_CHECK(semaphoreResult);
}

void VulkanFrameSync::Shutdown() {
    vkDestroyFence(device->LogicalDevice(), fence, nullptr);
    fence = nullptr;

    vkDestroySemaphore(device->LogicalDevice(), acquireSemaphore, nullptr);
    acquireSemaphore = nullptr;
}

void VulkanFrameSync::WaitOnFence() const {
    vkWaitForFences(device->LogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
}

void VulkanFrameSync::ResetFence() const {
    vkResetFences(device->LogicalDevice(), 1, &fence);
}
