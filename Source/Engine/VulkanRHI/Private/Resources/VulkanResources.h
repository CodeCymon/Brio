// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>

#include "RHIResources.h"

class VulkanDevice;
typedef struct VmaAllocation_T* VmaAllocation;

//  -   -   -   -   -   -   -
//  Textures
//  -   -   -   -   -   -   -

class VulkanTexture final : public RHITexture {
public:
    ~VulkanTexture() override;

    [[nodiscard]] VkImage Image() const { return image; }
    [[nodiscard]] VkImageView DefaultView() const { return defaultView; }

private:
    VulkanTexture(VulkanDevice* device, RHITextureDesc const &desc, VkImage image, VkImageView view,
                  VmaAllocation allocation, bool bExternalMemory);

    friend class VulkanRHI;

private:
    VkImage image;
    VkImageView defaultView;
    VmaAllocation allocation;
    bool bExternalMemory;
    VulkanDevice* device;
};


class VulkanShaderResource {
public:
    virtual ~VulkanShaderResource();

    [[nodiscard]] VkShaderModule Module() const { return module; }

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

//  -   -   -   -   -   -   -
//  Pipelines
//  -   -   -   -   -   -   -

class VulkanGraphicsPipeline final : public RHIGraphicsPipeline {
public:
    ~VulkanGraphicsPipeline() override;

    [[nodiscard]] VkPipeline Pipeline() const { return pipeline; }
    [[nodiscard]] VkPipelineLayout Layout() const { return layout; }

private:
    VulkanGraphicsPipeline(VulkanDevice* device, RHIGraphicsPipelineDesc const &inDesc,
                           VkPipeline pipeline, VkPipelineLayout layout)
        : RHIGraphicsPipeline(inDesc), pipeline(pipeline), layout(layout), device(device) {}

    friend class VulkanRHI;

private:
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VulkanDevice* device;
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

template<>
struct TVulkanResourceTraits<RHIGraphicsPipeline> {
    using ConcreteType = VulkanGraphicsPipeline;
};


template<typename RHIType>
static TVulkanResourceTraits<RHIType>::ConcreteType* ResourceCast(RHIType* resource) {
    return static_cast<TVulkanResourceTraits<RHIType>::ConcreteType*>(resource);
}