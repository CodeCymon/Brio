// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

#include <vulkan/vk_enum_string_helper.h>

#include "LogRHI.h"

void VulkanRHI::Initialize(NativeWindowData const &windowData) {
    instance.Initialize(true);
    surface.Initialize(&instance, windowData);
    device.Initialize(&instance, &surface);
    swapchain.Initialize(&device, &surface, {800, 450});

    for (auto& frameSync : frameSyncs)
        frameSync.Initialize(&device);

    for (auto& frameCmd : frameCmdData)
        frameCmd.Initialize(&device);
}

void VulkanRHI::Shutdown() {
    WaitForIdle();

    for (auto& frameCmd : frameCmdData)
        frameCmd.Shutdown();

    for (auto& frameSync : frameSyncs)
        frameSync.Shutdown();

    swapchain.Shutdown();
    device.Shutdown();
    surface.Shutdown();
    instance.Shutdown();
}

void VulkanRHI::WaitForIdle() {
    vkDeviceWaitIdle(device.LogicalDevice());
}

void VulkanRHI::OnResize(u32 width, u32 height) {
    if (width == 0 || height == 0) return;

    WaitForIdle();
    swapchain.Resize({width, height});
    LOG_VERBOSE(LogRHI, "Resized swapchain to: {}x{}", width, height);
}

RHIFrameContext VulkanRHI::BeginFrame() {
    VulkanFrameSync const& sync = frameSyncs[frameIndex];
    VulkanFrameCmdData const& cmdData = frameCmdData[frameIndex];

    sync.WaitOnFence();

    VkResult acquired = swapchain.AcquireNextImage(sync.GetAcquireSemaphore());
    if (acquired != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "Failed to acquire swapchain image: {}", string_VkResult(acquired));
    }

    sync.ResetFence();

    cmdData.BeginCommandBuffer();
    cmdList.BindActiveCommandBuffer(cmdData.Cmd());

    // TODO: remove
    {
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = device.GraphicsQueueFamilyIndex(),
            .dstQueueFamilyIndex = device.GraphicsQueueFamilyIndex(),
            .image = swapchain.Image(),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1,
            }
        };

        VkDependencyInfo dependencyInfo = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &imageBarrier
        };
        vkCmdPipelineBarrier2(cmdData.Cmd(), &dependencyInfo);
    }

    return {
        .cmdList = &cmdList,
        .frameIndex = frameIndex
    };
}

void VulkanRHI::EndFrame() {
    VulkanFrameSync const& sync = frameSyncs[frameIndex];
    VulkanFrameCmdData const& cmdData = frameCmdData[frameIndex];

    cmdData.EndCommandBuffer();

    VkCommandBuffer commands[] = {cmdData.Cmd()};
    VkSemaphore waitSemaphores[] = {sync.GetAcquireSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {swapchain.GetSubmitSemaphore()};
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = commands,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };
    vkQueueSubmit(device.GraphicsQueue(), 1, &submitInfo, sync.Fence());

    VkResult presented = swapchain.Present();
    if (presented != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "Presenting swapchain failed: {}", string_VkResult(presented));
    }

    frameIndex = (frameIndex + 1) % kMaxFramesInFlight;
}
