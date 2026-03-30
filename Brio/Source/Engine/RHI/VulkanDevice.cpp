#include "VulkanDevice.h"

#include <cstring>
#include <queue>
#include <unordered_set>

#include "VulkanCheck.h"
#include "Core/Asserts/Assert.h"
#include "RHI/VulkanUtils.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOG_ERROR(LogRHI, "{} - {}: {}", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_WARN(LogRHI, "{} - {}: {}", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, pCallbackData->pMessage);
    } else {
        LOG_INFO(LogRHI, "{} - {}: {}", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, pCallbackData->pMessage);
    }

    return VK_FALSE;
}

bool VulkanDevice::GPU::supportsSwapchain() const {
    ASSERT(handle, "GPU handle must be valid!");

    u32 count = 0;
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &count, nullptr);
    TArray<VkExtensionProperties> properties(count);
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &count, properties.data());

    for (auto const &extension : properties) {
        if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            return true;
        }
    }

    return false;
}

VulkanDevice::VulkanDevice() = default;

VulkanDevice::~VulkanDevice() = default;

void VulkanDevice::init(Config const &config) {
    createInstance(config.getExtensions, config.useValidation);
    if (config.useValidation) {
        createDebugger();
    }
    createSurface(config.getSurface);
    pickGPU();
    createDevice();
}

void VulkanDevice::shutdown() {
    destroyDevice();
    destroySurface();
    if (debugger) destroyDebugger();
    destroyInstance();
}

void VulkanDevice::createInstance(getPlatformExtensionsFn const &getPlatformExtensions, bool useValidation) {
    constexpr VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Brio",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "BrioEngine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_4
    };

    const TArray<const char *> extensions = getInstanceExtensions(getPlatformExtensions, useValidation);
    const TArray<const char *> layers = getInstanceLayers(useValidation);

    const VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<u32>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    VK_CHECK(result);
    LOG_INFO(LogRHI, "Instance created.");
}

void VulkanDevice::destroyInstance() {
    ASSERT(instance, "Instance must be valid to be destroyed!");
    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
    LOG_INFO(LogRHI, "Instance destroyed.");
}

TArray<const char *> VulkanDevice::getInstanceExtensions(getPlatformExtensionsFn const &getPlatformExtensions, bool useValidation) {
    TArray<const char *> extensions = getPlatformExtensions();
    ASSERT(!extensions.empty(), "Platform extensions must not be empty!");

    if (useValidation) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

TArray<const char *> VulkanDevice::getInstanceLayers(bool useValidation) {
    if (useValidation) {
        return {"VK_LAYER_KHRONOS_validation"};
    }
    return {};
}

void VulkanDevice::createDebugger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    VkResult result = INSTANCE_FUNCTION(vkCreateDebugUtilsMessengerEXT)(instance, &createInfo, nullptr, &debugger);
    VK_CHECK(result);
    LOG_INFO(LogRHI, "Debugger created.");
}

void VulkanDevice::destroyDebugger() {
    ASSERT(debugger, "Debugger must be valid to be destroyed!");
    INSTANCE_FUNCTION(vkDestroyDebugUtilsMessengerEXT)(instance, debugger, nullptr);
    debugger = nullptr;
    LOG_INFO(LogRHI, "Debugger destroyed.");
}

void VulkanDevice::createSurface(getPlatformSurfaceFn const &getSurface) {
    surface = static_cast<VkSurfaceKHR>(getSurface(instance));
    ASSERT(surface, "Surface must be valid!");
    LOG_INFO(LogRHI, "Surface created.");
}

void VulkanDevice::destroySurface() {
    ASSERT(surface, "Surface must be valid to be destroyed!");
    vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = nullptr;
    LOG_INFO(LogRHI, "Surface destroyed.");
}

void VulkanDevice::pickGPU() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        LOG_FATAL(LogRHI, "No Vulkan-compatible GPUs found!");
        return;
    }

    TArray<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    GPU fallback {};
    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        GPU current {};
        current.handle = physicalDevice;

        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &current.memoryProperties);

        queryQueueFamilies(physicalDevice);

        bool supportsGraphics = graphicsQueue.hasFamilyIndex;
        bool supportsPresentation = presentQueue.hasFamilyIndex;
        bool supportsSwapchain = current.supportsSwapchain();
        if (supportsGraphics && supportsPresentation && supportsSwapchain) {
            VkPhysicalDeviceProperties properties {};
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                gpu = current;
                LOG_INFO(LogRHI, "Selected GPU: {}", properties.deviceName);
                return;
            }

            fallback = current;
        }
    }

    if (fallback.handle) {
        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(fallback.handle, &properties);
        LOG_INFO(LogRHI, "Selected Fallback GPU: {}", properties.deviceName);
        return;
    }

    LOG_FATAL(LogRHI, "Found {} GPUs but none met the requirements.", deviceCount);
}


void VulkanDevice::queryQueueFamilies(VkPhysicalDevice physicalDevice) {
    ASSERT(physicalDevice, "Physical device must be valid!");
    ASSERT(surface, "Surface must be valid!");

    graphicsQueue.hasFamilyIndex = false;
    presentQueue.hasFamilyIndex = false;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    TArray<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (u32 i = 0; i < queueFamilyCount; i++) {
        if (!graphicsQueue.hasFamilyIndex && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueue.hasFamilyIndex = true;
            graphicsQueue.familyIndex = i;
        }

        VkBool32 canPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &canPresent);
        if (!presentQueue.hasFamilyIndex && canPresent == VK_TRUE) {
            presentQueue.hasFamilyIndex = true;
            presentQueue.familyIndex = i;
        }
    }
}

void VulkanDevice::createDevice() {
    ASSERT(gpu.handle, "GPU must be valid!");
    ASSERT(graphicsQueue.hasFamilyIndex, "Graphics queue must be available!");
    ASSERT(presentQueue.hasFamilyIndex, "Present queue must be available!");

    std::unordered_set<u32> uniqueFamilies;
    uniqueFamilies.insert(graphicsQueue.familyIndex);
    uniqueFamilies.insert(presentQueue.familyIndex);

    f32 priority = 1.0f;
    TArray<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (u32 family : uniqueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = family,
            .queueCount = 1,
            .pQueuePriorities = &priority,
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRendering = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
        .pNext = nullptr,
        .dynamicRendering = VK_TRUE
    };

    VkPhysicalDeviceSynchronization2Features synchronization2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .pNext = &dynamicRendering,
        .synchronization2 = VK_TRUE
    };

    VkPhysicalDeviceFeatures2 features2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &synchronization2,
        .features = {0}
    };

    TArray<const char*> deviceExtensions = getDeviceExtensions();

    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features2,
        .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<u32>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
    };

    VkResult result = vkCreateDevice(gpu.handle, &deviceInfo, nullptr, &device);
    VK_CHECK(result);

    vkGetDeviceQueue(device, graphicsQueue.familyIndex, 0, &graphicsQueue.handle);
    vkGetDeviceQueue(device, presentQueue.familyIndex, 0, &presentQueue.handle);

    LOG_INFO(LogRHI, "Logical Device created.");
}

void VulkanDevice::destroyDevice() {
    ASSERT(device, "Device must be valid!");
    vkDestroyDevice(device, nullptr);
    device = nullptr;
    LOG_INFO(LogRHI, "Logical Device destroyed.");
}

TArray<const char *> VulkanDevice::getDeviceExtensions() {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}