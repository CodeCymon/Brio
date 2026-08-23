// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "DynamicRHI.h"

#include <vma.h>

#include "VulkanExternalTextureRegistry.h"

#include "Vulkan/Bootstrapping/VulkanInstance.h"
#include "Vulkan/Bootstrapping/VulkanSurface.h"
#include "Vulkan/Bootstrapping/VulkanDevice.h"
#include "Vulkan/Bootstrapping/VulkanSwapchain.h"
#include "Vulkan/Bootstrapping/VulkanTimeline.h"
#include "Vulkan/Bootstrapping/VulkanFrameSync.h"
#include "Vulkan/Bootstrapping/VulkanFrameCmdData.h"

#include "Vulkan/VulkanCommandList.h"
#include "Vulkan/VulkanDeletionQueue.h"
#include "Vulkan/VulkanResourceContext.h"


class VulkanRHI final : public IDynamicRHI, public IVulkanExternalTextureRegistry {
public:
    void Initialize(NativeWindowData const &windowData) override;

    void Shutdown() override;

    void WaitForIdle() override;

    void OnResize(u32 width, u32 height) override;

    [[nodiscard]] RHIFrameContext BeginFrame() override;
    void EndFrame() override;

    [[nodiscard]] RHITextureRef CreateTexture(RHITextureDesc const &desc, char const* debugName) override;

    VulkanTexture* RegisterExternalTexture(RHITextureDesc const &desc, VkImage image, VkImageView defaultView) override;
    void UnregisterExternalTexture(VulkanTexture* texture) override;

private:
    VulkanInstance instance;
    VulkanSurface surface;
    VulkanDevice device;
    VulkanSwapchain swapchain;
    VulkanTimeline timeline;
    StaticArray<VulkanFrameSync, kMaxFramesInFlight> frameSyncs;
    StaticArray<VulkanFrameCmdData, kMaxFramesInFlight> frameCmdData;

    VulkanCommandList cmdList;

    VmaAllocator allocator {};
    VulkanDeletionQueue deletionQueue;
    VulkanResourceContext resourceContext {};
    SlabPool<VulkanTexture> texturePool;

    u32 frameIndex {0};
};
