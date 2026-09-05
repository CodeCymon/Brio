// Copyright (c) Simon Kirsch 2026.

#include "VulkanRHI.h"

#include "RHIResources.h"
#include "vk_mem_alloc.h"
#include "VulkanCheck.h"
#include "TranslationUtils/BufferTranslations.h"

VulkanBuffer::~VulkanBuffer() {
    device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::Buffer, buffer, allocation);
}

VulkanBuffer::VulkanBuffer(VulkanDevice* device, RHIBufferDesc const &desc, VkBuffer buffer,
    VmaAllocation allocation, u64 gpuAddress)
        : RHIBuffer(desc, gpuAddress), buffer(buffer), allocation(allocation), device(device) {}

VulkanMappedBuffer::~VulkanMappedBuffer() {
    device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::Buffer, buffer, allocation);
}

VulkanMappedBuffer::VulkanMappedBuffer(VulkanDevice* device, RHIBufferDesc const &desc,
                                       VkBuffer buffer, VmaAllocation allocation, u64 gpuAddress,
                                       void* ptr)
    : RHIMappedBuffer(desc, gpuAddress, ptr), buffer(buffer), allocation(allocation), device(device) {}

RHIBufferRef VulkanRHI::CreateBuffer(RHIBufferDesc const &desc, char const* debugName) {
    VkBufferCreateInfo bufferInfo = BufferTranslation::CreateInfoFromDesc(desc);
    VmaAllocationCreateInfo allocInfo { .usage = VMA_MEMORY_USAGE_AUTO };

    VkBuffer buffer{}; VmaAllocation allocation {};
    VkResult bufferResult = vmaCreateBuffer(device.Allocator(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    VK_CHECK(bufferResult);

    if (debugName) device.SetObjectDebugName(VK_OBJECT_TYPE_BUFFER, (u64)buffer, debugName);

    u64 address = 0;
    if (Contains(desc.usage,BufferUsage::ShaderAddressable)) {
        VkBufferDeviceAddressInfo addressInfo {.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = buffer};
        address = vkGetBufferDeviceAddress(device.LogicalDevice(), &addressInfo);
    }

    return new VulkanBuffer{&device, desc, buffer, allocation, address};
}

RHIMappedBufferRef VulkanRHI::CreateMappedBuffer(RHIBufferDesc const& desc, char const* debugName) {
    VkBufferCreateInfo bufferInfo = BufferTranslation::CreateInfoFromDesc(desc);
    VmaAllocationCreateInfo allocInfo {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
    };

    VkBuffer buffer{}; VmaAllocation allocation {}; VmaAllocationInfo allocationInfo {};
    VkResult bufferResult = vmaCreateBuffer(device.Allocator(), &bufferInfo, &allocInfo, &buffer, &allocation, &allocationInfo);
    VK_CHECK(bufferResult);

    if (debugName) device.SetObjectDebugName(VK_OBJECT_TYPE_BUFFER, (u64)buffer, debugName);

    void* ptr = allocationInfo.pMappedData;
    u64 address = 0;
    if (Contains(desc.usage,BufferUsage::ShaderAddressable)) {
        VkBufferDeviceAddressInfo addressInfo {.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = buffer};
        address = vkGetBufferDeviceAddress(device.LogicalDevice(), &addressInfo);
    }

    return new VulkanMappedBuffer{&device, desc, buffer, allocation, address, ptr};
}
