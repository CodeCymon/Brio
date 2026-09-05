// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan.h>

#include "Bootstrapping/VulkanLoader.h"
#include "Bootstrapping/VulkanTimeline.h"
#include "Resources/VulkanResources.h"

class VulkanTimeline;
class VulkanDevice;
class VulkanSurface;
class VulkanInstance;

struct VulkanDeviceFeatures {
    VulkanDeviceFeatures() {
        features14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        features14.pNext = nullptr;
        features13.pNext = &features14;
        features12.pNext = &features13;
        features11.pNext = &features12;
        features.pNext = &features11;
    }

    VkPhysicalDeviceFeatures2 features {};
    VkPhysicalDeviceVulkan11Features features11 {};
    VkPhysicalDeviceVulkan12Features features12 {};
    VkPhysicalDeviceVulkan13Features features13 {};
    VkPhysicalDeviceVulkan14Features features14 {};
};

class VulkanDeferredDeletionQueue {
public:
    enum class Type : u32 {
        Image,
        ImageView,
        ShaderModule,
        Pipeline,
        PipelineLayout,
    };

private:
    struct ResourceEntry {
        Type type;
        u64 handle;
        VmaAllocation allocation;
        u64 readyAt;
    };

public:
    VulkanDeferredDeletionQueue(VulkanDevice& device, VulkanTimeline const& timeline);

    template<typename T>
    void Enqueue(Type type, T handle, VmaAllocation allocation = nullptr) {
        EnqueueResource(type, reinterpret_cast<u64>(handle), allocation);
    }

    void Flush(u64 completedTimelineValue);
    void FlushAll();

private:
    void EnqueueResource(Type type, u64 handle, VmaAllocation allocation);
    void DestroyEntry(ResourceEntry const& entry) const;

    Array<ResourceEntry> pendingResources;

    VulkanDevice& device;
    VulkanTimeline const& timeline;
};

class VulkanDevice {
    struct QueueFamilyIndices {
        u32& graphics;
        u32& compute;
        u32& present;
    };
public:
    VulkanDevice(VulkanTimeline const& timeline);
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

    VulkanDeferredDeletionQueue& DeferredDeletionQueue() { return deferredDeletionQueue; }
    struct VmaAllocator_T* Allocator() const { return allocator; }
    VulkanShaderFactory& ShaderFactory() { return shaderFactory; }

    VulkanLoader& Loader() { return loader; }
    void SetObjectDebugName(VkObjectType type, u64 handle, const char* name);

    bool IsDebugEnabled() const;

private:
    void PickPhysicalDevice();
    void CreateLogicalDevice();

    static bool HasQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices outIndices);
    static bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, Array<const char*> const &requestedExtensions);

    static Array<const char*> RequiredExtensions();
    static VulkanDeviceFeatures RequiredFeatures();

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

    struct VmaAllocator_T* allocator{};
    VulkanDeferredDeletionQueue deferredDeletionQueue;
    VulkanShaderFactory shaderFactory {};

    VulkanLoader loader {};

    VulkanInstance const* instance {nullptr};
    VulkanSurface const* surface {nullptr};
};
