#include "Renderer.h"

#include <vulkan/vk_enum_string_helper.h>

#include "Core/Log/Logger.h"
#include "Platform/Platform.h"

void Renderer::init(Config const& config) {
    VulkanDevice::Config deviceConfig = {
        .platformSurfaceFn = [&](void* instance) -> void* {
            return config.platform.getSurface(instance);
        },
        .platformExtensionsFn = Platform::getExtensions,
        .validation = true
    };
    device_.init(deviceConfig);

    VulkanSwapchain::Config swapchainConfig = {
        .device = &device_,
        .width = config.width,
        .height = config.height
    };
    swapchain_.init(swapchainConfig);

    for (auto& frame : frames_) {
        frame.init({ .device = &device_ });
    }
}

void Renderer::shutdown() {
    vkDeviceWaitIdle(device_.device());

    for (auto& frame : frames_) {
        frame.shutdown();
    }
    swapchain_.shutdown();
    device_.shutdown();
}

void Renderer::render() {
    VulkanFrameContext& frame = frames_[frame_index_];

    frame.waitForFence();

    VkResult acquired = swapchain_.acquireNextImage(frame.acquireSemaphore());
    if (acquired != VK_SUCCESS) {
        LOG_WARN(LogRenderer, "Swapchain image acquire failed! {}", string_VkResult(acquired));
        return;
    }

    frame.resetFence();

    vkResetCommandBuffer(frame.cmd(), 0);

    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
    vkBeginCommandBuffer(frame.cmd(), &beginInfo);

    {
        {
            VkImageMemoryBarrier2 imageBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
                .srcAccessMask = VK_ACCESS_2_NONE,
                .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = device_.graphicsFamily(),
                .dstQueueFamilyIndex = device_.graphicsFamily(),
                .image = swapchain_.image(),
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
            vkCmdPipelineBarrier2(frame.cmd(), &dependencyInfo);
        }

        VkClearColorValue clearColor = {
            0.1, 0.7, 0.7, 1.0
        };
        VkImageSubresourceRange subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };
        vkCmdClearColorImage(
            frame.cmd(),
            swapchain_.image(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            &clearColor,
            1,
            &subresourceRange
        );

        {
            VkImageMemoryBarrier2 imageBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
                .dstAccessMask = VK_ACCESS_2_NONE,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = device_.graphicsFamily(),
                .dstQueueFamilyIndex = device_.graphicsFamily(),
                .image = swapchain_.image(),
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
            vkCmdPipelineBarrier2(frame.cmd(), &dependencyInfo);
        }
    }

    vkEndCommandBuffer(frame.cmd());


    VkCommandBuffer cmds[] = { frame.cmd() };
    VkSemaphore waitSemaphores[] = { frame.acquireSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { swapchain_.submitSemaphore() };
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = cmds,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };
    vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, frame.fence());

    VkResult presented = swapchain_.present();
    if (presented != VK_SUCCESS) {
        LOG_WARN(LogRenderer, "Failed to present swapchain! {}", string_VkResult(presented));
    }

    frame_index_ = (frame_index_ + 1) % FRAMES_IN_FLIGHT;
}

bool Renderer::onResize(u32 width, u32 height) {
    if (width == 0 || height == 0) {
        return true;
    }
    LOG_DEBUG(LogRenderer, "Resized");
    vkDeviceWaitIdle(device_.device());
    swapchain_.resize(width, height);
    return true;
}

