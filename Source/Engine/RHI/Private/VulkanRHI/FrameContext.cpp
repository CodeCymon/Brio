#include "FrameContext.h"

#include "Log/Assert.h"
#include "VulkanRHI/Device.h"
#include "VulkanRHI/VulkanCheck.h"

void VulkanFrameContext::Initialize(VulkanDevice const* deviceRef) {
    ASSERT(deviceRef);
    device = deviceRef;

    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
}

void VulkanFrameContext::Shutdown() {
    DestroySyncObjects();
    DestroyCommandBuffer();
    DestroyCommandPool();
}

void VulkanFrameContext::WaitForFence() const {
    vkWaitForFences(device->LogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
}

void VulkanFrameContext::ResetFence() const {
    vkResetFences(device->LogicalDevice(), 1, &fence);
}

void VulkanFrameContext::CreateCommandPool() {
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device->GraphicsFamilyIndex()
    };

    VkResult result = vkCreateCommandPool(device->LogicalDevice(), &poolInfo, nullptr, &commandPool);
    VK_CHECK(result);
}

void VulkanFrameContext::CreateCommandBuffer() {
    VkCommandBufferAllocateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkResult result = vkAllocateCommandBuffers(device->LogicalDevice(), &bufferInfo, &commandBuffer);
    VK_CHECK(result);
}

void VulkanFrameContext::CreateSyncObjects() {
    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkResult fenceResult = vkCreateFence(device->LogicalDevice(), &fenceInfo, nullptr, &fence);
    VK_CHECK(fenceResult);

    VkSemaphoreCreateInfo semInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkResult semResult = vkCreateSemaphore(device->LogicalDevice(), &semInfo, nullptr, &acquireSemaphore);
    VK_CHECK(semResult);
}

void VulkanFrameContext::DestroyCommandPool() {
    vkDestroyCommandPool(device->LogicalDevice(), commandPool, nullptr);
    commandPool = nullptr;
}

void VulkanFrameContext::DestroyCommandBuffer() {
    vkFreeCommandBuffers(device->LogicalDevice(), commandPool, 1, &commandBuffer);
    commandBuffer = nullptr;
}

void VulkanFrameContext::DestroySyncObjects() {
    vkDestroySemaphore(device->LogicalDevice(), acquireSemaphore, nullptr);
    acquireSemaphore = nullptr;

    vkDestroyFence(device->LogicalDevice(), fence, nullptr);
    fence = nullptr;
}
