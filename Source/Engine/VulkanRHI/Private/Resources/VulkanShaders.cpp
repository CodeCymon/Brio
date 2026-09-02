// Copyright (c) Simon Kirsch 2026.

#include "VulkanResources.h"
#include "VulkanRHI.h"

VulkanShaderResource::~VulkanShaderResource() {
    device->DeferredDeletionQueue().Enqueue(VulkanDeferredDeletionQueue::Type::ShaderModule, module);
}

VulkanShaderResource::VulkanShaderResource(VulkanDevice* device, VkShaderModule module)
    : module(module), device(device) {}


template<typename VulkanShaderType>
VulkanShaderType* VulkanShaderFactory::CreateShader(Array<u32> const &byteCode, VulkanDevice* device) {
    VkShaderModuleCreateInfo info {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = byteCode.Size() * sizeof(u32),
        .pCode = byteCode.Data()
    };
    VkShaderModule module;
    vkCreateShaderModule(device->LogicalDevice(), &info, nullptr, &module);

    return new VulkanShaderType{device, module};
}


RHIVertexShaderRef VulkanRHI::CreateVertexShader(RHIShaderDesc const &desc) {
    return RHIVertexShaderRef{device.ShaderFactory().CreateShader<VulkanVertexShader>(desc.codeBytes, &device)};
}

RHIPixelShaderRef VulkanRHI::CreatePixelShader(RHIShaderDesc const &desc) {
    return RHIPixelShaderRef{device.ShaderFactory().CreateShader<VulkanPixelShader>(desc.codeBytes, &device)};
}