// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "RHIResources.h"
#include "Containers/SlabPool.h"

class VulkanDevice;

class VulkanTexture final : public RHITexture {
public:
    ~VulkanTexture() override;

    [[nodiscard]] VkImage Image() const { return image; }

private:
    VulkanTexture(VulkanDevice* device, RHITextureDesc const &desc, VkImage image,
                  VmaAllocation allocation, bool bExternalMemory);

    friend class VulkanRHI;

private:
    VkImage image {};
    VmaAllocation allocation {};
    bool bExternalMemory {false};
    VulkanDevice* device {};
};


class VulkanShaderResource {
public:
    virtual ~VulkanShaderResource();

protected:
    VulkanShaderResource(VulkanDevice* device, VkShaderModule module);

    VkShaderModule module;
    VulkanDevice* device;
};

template<class RHIShaderType>
class TVulkanShaderBase final : public RHIShaderType, public VulkanShaderResource {
private:
    TVulkanShaderBase(VulkanDevice* device, VkShaderModule module)
        : RHIShaderType(), VulkanShaderResource(device, module) {}

    friend class VulkanShaderFactory;
};

using VulkanVertexShader = TVulkanShaderBase<RHIVertexShader>;
using VulkanPixelShader = TVulkanShaderBase<RHIPixelShader>;
using VulkanComputeShader = TVulkanShaderBase<RHIComputeShader>;

class VulkanShaderFactory {
public:
    template<typename VulkanShaderType>
    VulkanShaderType* CreateShader(Array<u32> const& byteCode, VulkanDevice* device);
};



template<class T>
struct TVulkanResourceTraits {};

template<>
struct TVulkanResourceTraits<RHITexture> {
    using ConcreteType = VulkanTexture;
};

template<>
struct TVulkanResourceTraits<RHIVertexShader> {
    using ConcreteType = VulkanVertexShader;
};

template<>
struct TVulkanResourceTraits<RHIPixelShader> {
    using ConcreteType = VulkanPixelShader;
};

template<>
struct TVulkanResourceTraits<RHIComputeShader> {
    using ConcreteType = VulkanComputeShader;
};



template<typename RHIType>
static TVulkanResourceTraits<RHIType>::ConcreteType* ResourceCast(RHIType* resource) {
    return static_cast<TVulkanResourceTraits<RHIType>::ConcreteType*>(resource);
}