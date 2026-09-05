// Copyright (c) Simon Kirsch 2026.

#include "VulkanDevice.h"

#include <cstring>

#include <vk_mem_alloc.h>

#include "Containers/Set.h"

#if PLATFORM_MACOS
#include <vulkan/vulkan_beta.h>
#endif

#include "LogVulkan.h"
#include "VulkanCheck.h"
#include "Bootstrapping/VulkanInstance.h"
#include "Bootstrapping/VulkanSurface.h"

VulkanDevice::VulkanDevice(VulkanTimeline const& timeline) : deferredDeletionQueue(*this, timeline) {}

void VulkanDevice::Initialize(VulkanInstance const* inInstance, VulkanSurface const* inSurface) {
    instance = inInstance;
    surface = inSurface;

    PickPhysicalDevice();
    CreateLogicalDevice();

    loader.LoadFunctions(logicalDevice);

    VmaAllocatorCreateInfo allocatorInfo = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = physicalDevice,
        .device = logicalDevice,
        .instance = instance->Instance(),
        .vulkanApiVersion = VK_API_VERSION_1_4,
    };
    vmaCreateAllocator(&allocatorInfo, &allocator);
}

void VulkanDevice::Shutdown() {
    vmaDestroyAllocator(allocator);

    vkDestroyDevice(logicalDevice, nullptr);
    logicalDevice = nullptr;
}

bool VulkanDevice::IsDebugEnabled() const {
    return instance->IsValidationEnabled();
}

void VulkanDevice::SetObjectDebugName(VkObjectType type, u64 handle, const char* name) {
    if (IsDebugEnabled() == false) return;

    VkDebugUtilsObjectNameInfoEXT nameInfo {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = type,
        .objectHandle = handle,
        .pObjectName = name
    };
    loader.SetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
}

void VulkanDevice::PickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->Instance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        LOG_FATAL(LogVulkan, "No compatible GPUs found!");
    }

    Array<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance->Instance(), &deviceCount, devices.Data());

    Array requiredExtensions = RequiredExtensions();
    VkSurfaceKHR vkSurface = surface->Surface();

    VkPhysicalDevice fallbackPhysicalDevice {};
    for (VkPhysicalDevice device : devices) {
        if (IsDeviceSuitable(device, requiredExtensions)
            && HasQueueFamilies(device, vkSurface, {graphicsQueueFamilyIndex, computeQueueFamilyIndex, presentQueueFamilyIndex})) {
            VkPhysicalDeviceProperties properties {};
            vkGetPhysicalDeviceProperties(device, &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                physicalDevice = device;
                vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
                LOG_INFO(LogVulkan, "Selected GPU: {}", properties.deviceName);
                return;
            }

            fallbackPhysicalDevice = device;
        }
    }

    if (fallbackPhysicalDevice) {
        physicalDevice = fallbackPhysicalDevice;
        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(fallbackPhysicalDevice, &properties);
        vkGetPhysicalDeviceMemoryProperties(fallbackPhysicalDevice, &memoryProperties);
        LOG_INFO(LogVulkan, "Selected fallback-GPU: {}", properties.deviceName);
        return;
    }

    LOG_FATAL(LogVulkan, "Found {} GPUs but none met the requirements.", deviceCount);
}

void VulkanDevice::CreateLogicalDevice() {
    Set<u32> uniqueQueueFamilies {};
    uniqueQueueFamilies.Add(graphicsQueueFamilyIndex);
    uniqueQueueFamilies.Add(computeQueueFamilyIndex);
    uniqueQueueFamilies.Add(presentQueueFamilyIndex);

    ASSERTM(uniqueQueueFamilies.Size() == 1, "Concurrent/Async Queues are not implemented!");

    f32 priority = 1.0f;
    Array<VkDeviceQueueCreateInfo> queueCreateInfos {};
    for (u32 queueFamilyIndex : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &priority,
        };
        queueCreateInfos.Add(queueCreateInfo);
    }

    VulkanDeviceFeatures features = RequiredFeatures();
    Array extensions = RequiredExtensions();

    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features.features,
        .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.Size()),
        .pQueueCreateInfos = queueCreateInfos.Data(),
        .enabledExtensionCount = static_cast<u32>(extensions.Size()),
        .ppEnabledExtensionNames = extensions.Data(),
    };

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
    VK_CHECK(result);

    vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, &computeQueue);
    vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &presentQueue);
}

bool VulkanDevice::HasQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices outIndices) {
    bool bHasGraphics = false;
    bool bHasCompute = false;
    bool bHasPresentation = false;

    u32 familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
    Array<VkQueueFamilyProperties> queueFamilyProperties(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueFamilyProperties.Data());

    for (u32 i = 0; i < familyCount; i++) {
        if (!bHasGraphics && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            outIndices.graphics = i;
            bHasGraphics = true;
        }

        if (!bHasCompute && queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            outIndices.compute = i;
            bHasCompute = true;
        }

        VkBool32 bCanPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &bCanPresent);
        if (!bHasPresentation && bCanPresent) {
            outIndices.present = i;
            bHasPresentation = true;
        }
    }

    return bHasGraphics && bHasCompute && bHasPresentation;
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice, Array<const char*> const& requestedExtensions) {
    u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    Array<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.Data());

    for (auto const& requestedExtension : requestedExtensions) {
        bool hasExtension = false;
        for (auto const& extension : availableExtensions) {
            if (strcmp(requestedExtension, extension.extensionName) == 0)
                hasExtension = true;
        }
        if (!hasExtension)
            return false;
    }

    return true;
}

Array<const char*> VulkanDevice::RequiredExtensions() {
    Array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#if PLATFORM_MACOS
    extensions.Add(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif
    return extensions;
}

VulkanDeviceFeatures VulkanDevice::RequiredFeatures() {
    VulkanDeviceFeatures features {};
    features.features13.synchronization2 = VK_TRUE;
    features.features13.dynamicRendering = VK_TRUE;
    features.features12.timelineSemaphore = VK_TRUE;

    features.features12.descriptorIndexing = VK_TRUE;
    features.features12.bufferDeviceAddress = VK_TRUE;

    features.features.features.shaderInt64 = VK_TRUE;

    return features;
}
