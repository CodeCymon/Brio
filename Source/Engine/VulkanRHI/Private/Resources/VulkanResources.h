// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "RHIResources.h"
#include "Containers/SlabPool.h"

struct VulkanResourceContext;
class VulkanTexture;

class VulkanTexture final : public RHITexture {
private:
    friend class SlabPool<VulkanTexture>;

    VulkanTexture(VulkanResourceContext* ctx, RHITextureDesc const &desc, VkImage image,
                  VmaAllocation allocation, VkImageView defaultView, bool bExternalMemory);

public:
    ~VulkanTexture() override;
    void Destroy() override;

    [[nodiscard]] VkImage Image() const { return image; }
    [[nodiscard]] VkImageView DefaultView() const { return defaultView; }

protected:
    void OnRefCountZero() override;

private:
    VkImage image {};
    VkImageView defaultView {};
    VmaAllocation allocation {};
    bool bExternalMemory {false};
    VulkanResourceContext* context {};
};


template<class T>
struct TVulkanResourceTraits {};

template<>
struct TVulkanResourceTraits<RHITexture> {
    using ConcreteType = VulkanTexture;
};


template<typename RHIType>
static inline TVulkanResourceTraits<RHIType>::ConcreteType* ResourceCast(RHIType* resource) {
    return static_cast<TVulkanResourceTraits<RHIType>::ConcreteType*>(resource);
}