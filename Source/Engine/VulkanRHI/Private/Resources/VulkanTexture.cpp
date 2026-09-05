// Copyright (c) Simon Kirsch 2026.

#include <vk_mem_alloc.h>

#include "VulkanCheck.h"
#include "VulkanResources.h"
#include "VulkanRHI.h"
#include "Bootstrapping/VulkanDevice.h"
#include "TranslationUtils/TextureTranslations.h"

VulkanTexture::~VulkanTexture() {
    if (!bExternalMemory) {
        device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::Image, image, allocation);
        device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::ImageView, defaultView);
    }
}

VulkanTexture::VulkanTexture(VulkanDevice* device, RHITextureDesc const &desc, VkImage image, VkImageView view,
                             VmaAllocation allocation, bool bExternalMemory)
    : RHITexture(desc), image(image), defaultView(view), allocation(allocation),
      bExternalMemory(bExternalMemory), device(device) {}

RHITextureRef VulkanRHI::CreateTexture(RHITextureDesc const &desc, char const* debugName) {
    VkImageCreateInfo imageInfo = ImageTranslation::CreateInfoFromDesc(desc);
    VmaAllocationCreateInfo allocInfo = { .usage = VMA_MEMORY_USAGE_AUTO };

    VkImage image {}; VmaAllocation allocation {};
    VkResult imageResult = vmaCreateImage(device.Allocator(), &imageInfo, &allocInfo, &image, &allocation, nullptr);
    VK_CHECK(imageResult);

    VkImageViewCreateInfo viewInfo  {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = imageInfo.format,
        .subresourceRange = {
            .aspectMask = ImageTranslation::ToVulkanImageAspectFlags(desc.format),
            .levelCount = 1,
            .layerCount = 1,
        }
    };

    VkImageView view {};
    VkResult viewResult = vkCreateImageView(device.LogicalDevice(), &viewInfo, nullptr, &view);
    VK_CHECK(viewResult);

    if (debugName && device.IsDebugEnabled()) {
        VkDebugUtilsObjectNameInfoEXT imageName {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_IMAGE,
            .objectHandle = reinterpret_cast<u64>(image),
            .pObjectName = debugName
        };
        reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(device.LogicalDevice(), "vkSetDebugUtilsObjectNameEXT"))(device.LogicalDevice(), &imageName);


        std::string viewStr = std::string(debugName) + std::string("_defaultView");
        VkDebugUtilsObjectNameInfoEXT viewName {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
            .objectHandle = reinterpret_cast<u64>(view),
            .pObjectName = viewStr.c_str(),
        };
        reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(device.LogicalDevice(), "vkSetDebugUtilsObjectNameEXT"))(device.LogicalDevice(), &viewName);
    }

    return new VulkanTexture{&device, desc, image, view, allocation, false};
}

VulkanTexture* VulkanRHI::RegisterExternalTexture(RHITextureDesc const &desc, VkImage image, VkImageView defaultView) {
    return new VulkanTexture{&device, desc, image, defaultView, nullptr, true};
}

void VulkanRHI::UnregisterExternalTexture(VulkanTexture* texture) {
    delete texture;
}