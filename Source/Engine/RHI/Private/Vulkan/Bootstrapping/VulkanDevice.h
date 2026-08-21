// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan.h>

#include "CoreMinimal.h"

class VulkanSurface;
class VulkanInstance;

class VulkanDevice {
    struct QueueFamilyIndices {
        u32& graphics;
        u32& compute;
        u32& present;
    };
public:
    VulkanDevice() = default;
    ~VulkanDevice() = default;

    NON_COPYABLE(VulkanDevice);
    NON_MOVEABLE(VulkanDevice);

    void Initialize(VulkanInstance const* inInstance, VulkanSurface const* inSurface);
    void Shutdown();

    [[nodiscard]] VkPhysicalDevice PhysicalDevice() const { return physicalDevice; }
    [[nodiscard]] VkPhysicalDeviceMemoryProperties const& MemoryProperties() const { return memoryProperties; }
    [[nodiscard]] VkDevice LogicalDevice() const { return logicalDevice; }

    [[nodiscard]] VkQueue GraphicsQueue() const { return graphicsQueue; }
    [[nodiscard]] u32 GraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }
    [[nodiscard]] VkQueue ComputeQueue() const { return computeQueue; }
    [[nodiscard]] u32 ComputeQueueFamilyIndex() const { return computeQueueFamilyIndex; }
    [[nodiscard]] VkQueue PresentQueue() const { return presentQueue; }
    [[nodiscard]] u32 PresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }

private:
    void PickPhysicalDevice();
    void CreateLogicalDevice();

    static bool HasQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices outIndices);
    static bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, Array<const char*> const &requestedExtensions);

    static Array<const char*> RequiredExtensions();

private:
    VkPhysicalDevice physicalDevice {};
    VkPhysicalDeviceMemoryProperties memoryProperties {};
    VkDevice logicalDevice {};

    VkQueue graphicsQueue {};
    u32 graphicsQueueFamilyIndex {};
    VkQueue computeQueue {};
    u32 computeQueueFamilyIndex {};
    VkQueue presentQueue {};
    u32 presentQueueFamilyIndex {};

    VulkanInstance const* instance {nullptr};
    VulkanSurface const* surface {nullptr};
};
