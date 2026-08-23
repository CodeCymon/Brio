// Copyright (c) Simon Kirsch 2026.

#include "VulkanDeletionQueue.h"

void VulkanDeletionQueue::Enqueue(RHIResource* resource, u64 readyAtTimelineValue) {
    pending.Add({resource, readyAtTimelineValue});
}

void VulkanDeletionQueue::Flush(u64 completedTimelineValue) {
    pending.RemoveIf([&](Entry const& e) {
        if (e.readyAt > completedTimelineValue) return false;
        e.resource->Destroy();
        return true;
    });
}

void VulkanDeletionQueue::FlushAll() {
    for (auto& entry : pending)
        entry.resource->Destroy();
    pending.Clear();
}
