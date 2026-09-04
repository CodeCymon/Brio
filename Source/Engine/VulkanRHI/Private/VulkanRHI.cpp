// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

#include <vulkan/vk_enum_string_helper.h>

#include "LogVulkan.h"
#include "TranslationUtils/TextureTranslations.h"

#if BUILD_DEBUG
static constexpr bool bUseValidation = true;
#else
static constexpr bool bUseValidation = false;
#endif

void VulkanRHI::Initialize(NativeWindowData const &windowData, UIntPoint const& initialExtent) {
    instance.Initialize(bUseValidation);
    surface.Initialize(&instance, windowData);
    device.Initialize(&instance, &surface);
    swapchain.Initialize(&device, &surface, this, initialExtent);

    timeline.Initialize(&device);


    for (auto& frameSync : frameSyncs)
        frameSync.Initialize(&device);

    for (auto& frameCmd : frameCmdData)
        frameCmd.Initialize(&device);
}

void VulkanRHI::Shutdown() {
    WaitForIdle();

    device.DeferredDeletionQueue().FlushAll();

    for (auto& frameCmd : frameCmdData)
        frameCmd.Shutdown();

    for (auto& frameSync : frameSyncs)
        frameSync.Shutdown();

    timeline.Shutdown();

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
    LOG_VERBOSE(LogVulkan, "Resized swapchain to: {}x{}", width, height);
}

RHIFrameContext VulkanRHI::BeginFrame() {
    VulkanFrameSync& sync = frameSyncs[frameIndex];
    VulkanFrameCmdData const& cmdData = frameCmdData[frameIndex];

    timeline.WaitUntil(sync.timelineWaitValue);
    device.DeferredDeletionQueue().Flush(sync.timelineWaitValue);

    VkResult acquired = swapchain.AcquireNextImage(sync.GetAcquireSemaphore());
    if (acquired != VK_SUCCESS) {
        LOG_WARNING(LogVulkan, "Failed to acquire swapchain image: {}", string_VkResult(acquired));
    }

    cmdData.BeginCommandBuffer();
    cmdList.BindActiveCommandBuffer(cmdData.Cmd());

    return RHIFrameContext{
        .cmdList = &cmdList,
        .backBuffer = swapchain.CurrentTexture(),
        .frameIndex = frameIndex
    };
}

void VulkanRHI::EndFrame() {
    VulkanFrameSync& sync = frameSyncs[frameIndex];
    VulkanFrameCmdData const& cmdData = frameCmdData[frameIndex];

    cmdData.EndCommandBuffer();

    u64 const signalValue = timeline.NextSignalValue();
    sync.timelineWaitValue = signalValue;

    VkCommandBufferSubmitInfo cmdInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cmdData.Cmd()
    };
    VkSemaphoreSubmitInfo waitInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = sync.GetAcquireSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
        }
    };
    VkSemaphoreSubmitInfo signalInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = swapchain.GetSubmitSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
        },
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = timeline.Semaphore(),
            .value = signalValue,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
        }
    };

    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = waitInfos,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdInfo,
        .signalSemaphoreInfoCount = 2,
        .pSignalSemaphoreInfos = signalInfos,
    };
    vkQueueSubmit2(device.GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

    VkResult presented = swapchain.Present();
    if (presented != VK_SUCCESS) {
        LOG_WARNING(LogVulkan, "Presenting swapchain failed: {}", string_VkResult(presented));
    }

    frameIndex = (frameIndex + 1) % kMaxFramesInFlight;
}