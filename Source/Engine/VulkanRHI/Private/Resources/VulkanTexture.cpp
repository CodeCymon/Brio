// Copyright (c) Simon Kirsch 2026.

#include "VulkanResources.h"

#include "VulkanDeletionQueue.h"
#include "VulkanResourceContext.h"
#include "Bootstrapping/VulkanDevice.h"
#include "Bootstrapping/VulkanTimeline.h"

VulkanTexture::VulkanTexture(VulkanResourceContext* ctx, RHITextureDesc const &desc, VkImage image,
    VmaAllocation allocation, VkImageView defaultView, bool bExternalMemory)
        : RHITexture(desc), image(image), defaultView(defaultView), allocation(allocation),
          bExternalMemory(bExternalMemory), context(ctx) {}

VulkanTexture::~VulkanTexture() {
    if (bExternalMemory) return;

    if (defaultView) vkDestroyImageView(context->device->LogicalDevice(), defaultView, nullptr);
    if (image) vmaDestroyImage(*context->allocator, image, allocation);
}

void VulkanTexture::Destroy() {
    context->texturePool->Destroy(this);
}

void VulkanTexture::OnRefCountZero() {
    if (bExternalMemory) {
        ASSERTM(false, "Tried to release a texture with external memory! -"
            " do not hold an RHITextureRef to it.");
        return;
    }
    context->deletionQueue->Enqueue(this, context->timeline->PendingSubmitValue());
}




