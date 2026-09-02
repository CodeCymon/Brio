// Copyright (c) Simon Kirsch 2026.

#include "VulkanResources.h"
#include "VulkanRHI.h"
#include "Bootstrapping/VulkanDevice.h"
#include "TranslationUtils/TextureTranslations.h"

VulkanTexture::~VulkanTexture() {
    if (!bExternalMemory)
        device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::Image, image, allocation);
}

VulkanTexture::VulkanTexture(VulkanDevice* device, RHITextureDesc const &desc, VkImage image,
                             VmaAllocation allocation, bool bExternalMemory)
    : RHITexture(desc), image(image), allocation(allocation), bExternalMemory(bExternalMemory),
      device(device) {}

RHITextureRef VulkanRHI::CreateTexture(RHITextureDesc const &desc, char const* debugName) {
    VkImageCreateInfo imageInfo = ImageTranslation::CreateInfoFromTextureDesc(desc);
    VmaAllocationCreateInfo allocInfo = { .usage = VMA_MEMORY_USAGE_AUTO };

    VkImage image {}; VmaAllocation allocation {};
    vmaCreateImage(device.Allocator(), &imageInfo, &allocInfo, &image, &allocation, nullptr);
    // TODO: debug naming
    auto* tex = new VulkanTexture{&device, desc, image, allocation, false};
    return RHITextureRef{tex};
}

VulkanTexture* VulkanRHI::RegisterExternalTexture(RHITextureDesc const &desc, VkImage image) {
    return new VulkanTexture{&device, desc, image, nullptr, true};
}

void VulkanRHI::UnregisterExternalTexture(VulkanTexture* texture) {
    delete texture;
}