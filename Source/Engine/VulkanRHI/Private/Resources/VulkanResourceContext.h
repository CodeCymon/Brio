// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "Containers/SlabPool.h"
#include "VulkanResources.h"

typedef struct VmaAllocator_T* VmaAllocator;
class VulkanDevice;
class VulkanTimeline;
class VulkanDeletionQueue;

struct VulkanResourceContext {
    VulkanDevice* device;
    VmaAllocator* allocator;
    VulkanTimeline* timeline;
    VulkanDeletionQueue* deletionQueue;
    SlabPool<VulkanTexture>* texturePool;
};
