#include "FrameContext.h"

#include "Device.h"
#include "VulkanCheck.h"
#include "Core/Asserts/Assert.h"

void VulkanFrameContext::init(Config const &config) {
    ASSERT(config.device, "Device must be valid!");
    device_ = config.device;
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
    LOG_INFO(LogRHI, "Frame Context initialized.");
}

void VulkanFrameContext::shutdown() {
    destroySyncObjects();
    destroyCommandBuffer();
    destroyCommandPool();
    LOG_INFO(LogRHI, "Frame Context shutdown.");
}

void VulkanFrameContext::waitForFence() const {
    vkWaitForFences(device_->device(), 1, &fence_, VK_TRUE, UINT64_MAX);
}

void VulkanFrameContext::resetFence() const {
    vkResetFences(device_->device(), 1, &fence_);
}

void VulkanFrameContext::createCommandPool() {
    VkCommandPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device_->graphicsFamily()
    };
    VkResult result = vkCreateCommandPool(device_->device(), &createInfo, nullptr, &command_pool_);
    VK_CHECK(result, "Command Pool creation failed!");
    LOG_DETAIL(LogRHI, "Frame Command Pool created.");
}

void VulkanFrameContext::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VkResult result = vkAllocateCommandBuffers(device_->device(), &allocInfo, &command_buffer_);
    VK_CHECK(result, "Failed to allocate command buffer!");
    LOG_DETAIL(LogRHI, "Frame Command Buffer(s) created.");
}

void VulkanFrameContext::createSyncObjects() {
    {
        VkFenceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };
        VkResult result = vkCreateFence(device_->device(), &createInfo, nullptr, &fence_);
        VK_CHECK(result, "failed to create frame fence!");
    }

    {
        VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VkResult result = vkCreateSemaphore(device_->device(), &createInfo, nullptr, &acquire_semaphore_);
        VK_CHECK(result, "Failed to create frame semaphore!");
    }
    LOG_DETAIL(LogRHI, "Frame Sync Objects created.");
}

void VulkanFrameContext::destroyCommandPool() {
    vkDestroyCommandPool(device_->device(), command_pool_, nullptr);
    command_pool_ = nullptr;
    LOG_DETAIL(LogRHI, "Frame Command Pool destroyed.");
}

void VulkanFrameContext::destroyCommandBuffer() {
    vkFreeCommandBuffers(device_->device(), command_pool_, 1, &command_buffer_);
    command_buffer_ = nullptr;
    LOG_DETAIL(LogRHI, "Frame Command Buffer destroyed.");
}

void VulkanFrameContext::destroySyncObjects() {
    vkDestroySemaphore(device_->device(), acquire_semaphore_, nullptr);
    acquire_semaphore_ = nullptr;

    vkDestroyFence(device_->device(), fence_, nullptr);
    fence_ = nullptr;

    LOG_DETAIL(LogRHI, "Frame Sync Objects destroyed.");
}
