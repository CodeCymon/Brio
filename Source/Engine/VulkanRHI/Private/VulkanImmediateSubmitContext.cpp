// Copyright (c) Simon Kirsch 2026.

#include "VulkanImmediateSubmitContext.h"

#include "Bootstrapping/VulkanDevice.h"

VulkanImmediateSubmitContext::VulkanImmediateSubmitContext(VulkanDevice const& device) : device(&device) {}

bool VulkanImmediateSubmitContext::Initialize() {
    VkCommandPoolCreateInfo poolInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = device->GraphicsQueueFamilyIndex()
    };
    vkCreateCommandPool(device->LogicalDevice(), &poolInfo, nullptr, &pool);

    VkCommandBufferAllocateInfo bufferAllocInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    vkAllocateCommandBuffers(device->LogicalDevice(), &bufferAllocInfo, &cmd);

    VkFenceCreateInfo fenceInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };
    vkCreateFence(device->LogicalDevice(), &fenceInfo, nullptr, &fence);

    return true;
}

void VulkanImmediateSubmitContext::Shutdown() {
    vkDestroyFence(device->LogicalDevice(), fence, nullptr);
    fence = VK_NULL_HANDLE;
    vkFreeCommandBuffers(device->LogicalDevice(), pool, 1, &cmd);
    cmd = VK_NULL_HANDLE;
    vkDestroyCommandPool(device->LogicalDevice(), pool, nullptr);
    pool = VK_NULL_HANDLE;
}

