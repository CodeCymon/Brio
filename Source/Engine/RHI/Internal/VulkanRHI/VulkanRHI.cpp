#include "VulkanRHI.h"

#include <vulkan/vk_enum_string_helper.h>

#include "RHIMinimal.h"

VulkanRHI::VulkanRHI() = default;

VulkanRHI::~VulkanRHI() = default;

void VulkanRHI::Initialize(FNativeWindowHandle const &windowHandle) {
    device.Initialize({.windowHandle = windowHandle, .bValidationEnabled = true});

    swapchain.Initialize({&device, 800, 450});

    for (auto& frame: frames)
        frame.Initialize(&device);


    LOG_INFO(LogRHI, "VulkanRHI initialized.");
}

void VulkanRHI::Shutdown() {

    vkDeviceWaitIdle(device.LogicalDevice());

    for (auto& frame: frames)
        frame.Shutdown();

    swapchain.Shutdown();

    device.Shutdown();

    LOG_INFO(LogRHI, "VulkanRHI shutdown.");
}

void VulkanRHI::OnResize(u32 width, u32 height) {
    if (width == 0 || height == 0)
        return;

    vkDeviceWaitIdle(device.LogicalDevice());
    swapchain.Resize(width, height);
}

FRHIFrameContext VulkanRHI::BeginFrame() {
    VulkanFrameContext& frame = frames[frameIndex];

    frame.WaitForFence();

    VkResult acquired = swapchain.AcquireNextImage(frame.GetAcquireSemaphore());
    if (acquired != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "AcquireNextImage failed! {}", string_VkResult(acquired));
    }

    frame.ResetFence();

    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    vkBeginCommandBuffer(frame.Cmd(), &beginInfo);

    FVulkanTexture* swapchainTexture = swapchain.GetCurrentTexture();

    return FRHIFrameContext{
        .swapchainImage = swapchainTexture,
        .frameIndex = frameIndex
    };
}

void VulkanRHI::EndFrame() {
    VulkanFrameContext& frame = frames[frameIndex];

    vkEndCommandBuffer(frame.Cmd());

    VkCommandBuffer commands[] = { frame.Cmd() };
    VkSemaphore waitSemaphores[] = { frame.GetAcquireSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { swapchain.GetSubmitSemaphore() };
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

    vkQueueSubmit(device.GraphicsQueue(), 1, &submitInfo, frame.Fence());

    VkResult presented = swapchain.Present();
    if (presented != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "Presenting swapchain failed! {}", string_VkResult(presented));
    }

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRHI::ClearTexture(FRHITexture* texture, FClearColor const &color) {

    VulkanFrameContext& frame = frames[frameIndex];

    FVulkanTexture* vkTexture = static_cast<FVulkanTexture*>(texture);

    {
        // TODO: remove
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .dstQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .image = vkTexture->image,
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
        vkCmdPipelineBarrier2(frame.Cmd(), &dependencyInfo);
    }

    {
        // TODO: remove
        VkClearColorValue clearColor = {
            color.r, color.g, color.b, color.a
        };
        VkImageSubresourceRange subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };
        vkCmdClearColorImage(
            frame.Cmd(),
            vkTexture->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            &clearColor,
            1,
            &subresourceRange
        );
    }

    {
        // TODO: remove
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .dstQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .image = vkTexture->image,
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
        vkCmdPipelineBarrier2(frame.Cmd(), &dependencyInfo);
    }
}
