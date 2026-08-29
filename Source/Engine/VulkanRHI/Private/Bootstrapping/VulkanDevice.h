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

private:
    struct DeviceFeatures {
        DeviceFeatures() {
            RebuildChain();
        }

        void RebuildChain() {
            features.pNext = &synchronization2Features;
            synchronization2Features.pNext = &timelineSemaphoreFeatures;
            timelineSemaphoreFeatures.pNext = &dynamicRenderingFeatures;
            dynamicRenderingFeatures.pNext = &descriptorIndexingFeatures;
            descriptorIndexingFeatures.pNext = &bufferDeviceAddressFeatures;
            bufferDeviceAddressFeatures.pNext = &rayQueryFeatures;
            rayQueryFeatures.pNext = &accelerationStructureFeatures;
        }

        DeviceFeatures(DeviceFeatures&& o) noexcept
        : features(o.features)
        , synchronization2Features(o.synchronization2Features)
        , timelineSemaphoreFeatures(o.timelineSemaphoreFeatures)
        , dynamicRenderingFeatures(o.dynamicRenderingFeatures)
        , descriptorIndexingFeatures(o.descriptorIndexingFeatures)
        , bufferDeviceAddressFeatures(o.bufferDeviceAddressFeatures)
        , rayQueryFeatures(o.rayQueryFeatures)
        , accelerationStructureFeatures(o.accelerationStructureFeatures)
        {
            RebuildChain();
        }

        DeviceFeatures& operator=(DeviceFeatures&& o) noexcept {
            features = o.features;
            synchronization2Features = o.synchronization2Features;
            timelineSemaphoreFeatures = o.timelineSemaphoreFeatures;
            dynamicRenderingFeatures = o.dynamicRenderingFeatures;
            descriptorIndexingFeatures = o.descriptorIndexingFeatures;
            bufferDeviceAddressFeatures = o.bufferDeviceAddressFeatures;
            rayQueryFeatures = o.rayQueryFeatures;
            accelerationStructureFeatures = o.accelerationStructureFeatures;
            RebuildChain();
            return *this;
        }

        NON_COPYABLE(DeviceFeatures);

        VkPhysicalDeviceFeatures2 features {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
        };

        VkPhysicalDeviceSynchronization2Features synchronization2Features {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES
        };
        VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES
        };
        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES
        };

        VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES
        };
        VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES
        };

        VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR
        };
        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR
        };
    };

    static DeviceFeatures RequiredFeatures();
};
