// Copyright (c) Simon Kirsch 2026.

#include <vk_mem_alloc.h>
#include "VulkanDevice.h"
#include "Bootstrapping/VulkanTimeline.h"

VulkanDeferredDeletionQueue::VulkanDeferredDeletionQueue(VulkanDevice& device, VulkanTimeline const& timeline)
    : device(device), timeline(timeline) {}

void VulkanDeferredDeletionQueue::Flush(u64 completedTimelineValue) {
    for (auto& entry : pendingResources) {
        if (entry.readyAt > completedTimelineValue) continue;
        DestroyEntry(entry);
    }

    pendingResources.RemoveIf([&](ResourceEntry const& e) {
       return e.readyAt <= completedTimelineValue;
    });
}

void VulkanDeferredDeletionQueue::FlushAll() {
    for (auto& entry : pendingResources) {
        DestroyEntry(entry);
    }
    pendingResources.Clear();
}

void VulkanDeferredDeletionQueue::EnqueueResource(Type type, u64 handle, VmaAllocation allocation) {
    pendingResources.Add({type, handle, allocation, timeline.PendingSubmitValue()});
}

void VulkanDeferredDeletionQueue::DestroyEntry(ResourceEntry const &entry) const {
    switch (entry.type) {
        case Type::Image:
            vmaDestroyImage(device.Allocator(), reinterpret_cast<VkImage>(entry.handle), entry.allocation);
            break;
        case Type::ImageView:
            vkDestroyImageView(device.LogicalDevice(), reinterpret_cast<VkImageView>(entry.handle), nullptr);
            break;
        case Type::Buffer:
            vmaDestroyBuffer(device.Allocator(), reinterpret_cast<VkBuffer>(entry.handle), entry.allocation);
            break;
        case Type::ShaderModule:
            vkDestroyShaderModule(device.LogicalDevice(), reinterpret_cast<VkShaderModule>(entry.handle), nullptr);
            break;
        case Type::Pipeline:
            vkDestroyPipeline(device.LogicalDevice(), reinterpret_cast<VkPipeline>(entry.handle), nullptr);
            break;
        case Type::PipelineLayout:
            vkDestroyPipelineLayout(device.LogicalDevice(), reinterpret_cast<VkPipelineLayout>(entry.handle), nullptr);
            break;
        default: ASSERT(false);
    }
}
