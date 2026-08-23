// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

#include <vulkan/vk_enum_string_helper.h>

#include "LogRHI.h"

void VulkanRHI::Initialize(NativeWindowData const &windowData) {
    resourceContext = {
        .device = &device,
        .allocator = &allocator,
        .timeline = &timeline,
        .deletionQueue = &deletionQueue,
        .texturePool = &texturePool,
    };

    instance.Initialize(true);
    surface.Initialize(&instance, windowData);
    device.Initialize(&instance, &surface);
    swapchain.Initialize(&device, &surface, this, {800, 450});

    timeline.Initialize(&device);


    for (auto& frameSync : frameSyncs)
        frameSync.Initialize(&device);

    for (auto& frameCmd : frameCmdData)
        frameCmd.Initialize(&device);

    VmaAllocatorCreateInfo allocatorInfo = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = device.PhysicalDevice(),
        .device = device.LogicalDevice(),
        .instance = instance.Instance(),
        .vulkanApiVersion = VK_API_VERSION_1_4,
    };
    vmaCreateAllocator(&allocatorInfo, &allocator);
}

void VulkanRHI::Shutdown() {
    WaitForIdle();

    deletionQueue.FlushAll();

    vmaDestroyAllocator(allocator);

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
    LOG_VERBOSE(LogRHI, "Resized swapchain to: {}x{}", width, height);
}

RHIFrameContext VulkanRHI::BeginFrame() {
    VulkanFrameSync& sync = frameSyncs[frameIndex];
    VulkanFrameCmdData const& cmdData = frameCmdData[frameIndex];

    timeline.WaitUntil(sync.timelineWaitValue);
    deletionQueue.Flush(sync.timelineWaitValue);

    VkResult acquired = swapchain.AcquireNextImage(sync.GetAcquireSemaphore());
    if (acquired != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "Failed to acquire swapchain image: {}", string_VkResult(acquired));
    }

    cmdData.BeginCommandBuffer();
    cmdList.BindActiveCommandBuffer(cmdData.Cmd());

    // TODO: remove
    {
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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

    return RHIFrameContext{
        .cmdList = &cmdList,
        .swapchainTexture = swapchain.CurrentTexture(),
        .frameIndex = frameIndex
    };
}

void VulkanRHI::EndFrame() {
    VulkanFrameSync& sync = frameSyncs[frameIndex];
    VulkanFrameCmdData const& cmdData = frameCmdData[frameIndex];

    // TODO: remove
    {
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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
        LOG_WARNING(LogRHI, "Presenting swapchain failed: {}", string_VkResult(presented));
    }

    frameIndex = (frameIndex + 1) % kMaxFramesInFlight;
}

RHITextureRef VulkanRHI::CreateTexture(RHITextureDesc const &desc, char const* debugName) {
    VkImageCreateInfo imageInfo = {/*from desc*/};
    VmaAllocationCreateInfo allocInfo = { .usage = VMA_MEMORY_USAGE_AUTO };

    VkImage image {}; VmaAllocation allocation {};
    vmaCreateImage(allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr);
    if (debugName) {
        VkDebugUtilsObjectNameInfoEXT nameInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_IMAGE,
            .objectHandle = reinterpret_cast<u64>(image),
            .pObjectName = debugName
        };
        // TODO: debug naming
        // vkSetDebugUtilsObjectNameEXT(device.LogicalDevice(), &nameInfo);
    }
    // TODO: create default view
    VulkanTexture* tex = texturePool.Allocate(&resourceContext, desc, image, allocation, nullptr, false);
    return RHITextureRef{tex};
}

VulkanTexture* VulkanRHI::RegisterExternalTexture(RHITextureDesc const &desc, VkImage image,
    VkImageView defaultView) {
    return texturePool.Allocate(&resourceContext, desc, image, nullptr, defaultView, true);
}

void VulkanRHI::UnregisterExternalTexture(VulkanTexture* texture) {
    texture->Destroy();
}
