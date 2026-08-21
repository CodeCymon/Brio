// Copyright (c) Simon Kirsch 2026.

#include "VulkanFrameCmdData.h"

#include "Vulkan/VulkanCheck.h"
#include "Vulkan/Bootstrapping/VulkanDevice.h"

void VulkanFrameCmdData::Initialize(VulkanDevice const* inDevice) {
    device = inDevice;

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device->GraphicsQueueFamilyIndex()
    };
    VkResult poolResult = vkCreateCommandPool(device->LogicalDevice(), &poolInfo, nullptr, &commandPool);
    VK_CHECK(poolResult);

    VkCommandBufferAllocateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkResult bufferResult = vkAllocateCommandBuffers(device->LogicalDevice(), &bufferInfo, &commandBuffer);
    VK_CHECK(bufferResult);
}

void VulkanFrameCmdData::Shutdown() {
    vkFreeCommandBuffers(device->LogicalDevice(), commandPool, 1, &commandBuffer);
    commandBuffer = nullptr;

    vkDestroyCommandPool(device->LogicalDevice(), commandPool, nullptr);
    commandPool = nullptr;
}

void VulkanFrameCmdData::BeginCommandBuffer() const {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void VulkanFrameCmdData::EndCommandBuffer() const {
    vkEndCommandBuffer(commandBuffer);
}
