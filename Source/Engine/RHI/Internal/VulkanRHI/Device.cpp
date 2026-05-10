#include "Device.h"

#include "RHIMinimal.h"
#include "VulkanRHI/VulkanCheck.h"

#include "NativeWindowHandle.h"

#include <cstring>
#include <unordered_set>

#if PLATFORM_LINUX
#include <vulkan/vulkan_wayland.h>
#endif

#define INSTANCE_FUNCTION(function) reinterpret_cast<PFN_##function>(vkGetInstanceProcAddr(instance, #function))

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             VkDebugUtilsMessengerCallbackDataEXT const* callbackData, void* userData) {

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOG_ERROR(LogRHI, "{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_WARNING(LogRHI, "{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
    } else {
        LOG_INFO(LogRHI, "{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
    }

    return VK_FALSE;
}

void VulkanDevice::Initialize(FConfig const &config) {
    bValidationEnabled = config.bValidationEnabled && CheckValidationSupport();

    CreateInstance();

    if (bValidationEnabled)
        CreateDebugger();

    CreateSurface(config.windowHandle);

    PickPhysicalDevice();
    CreateLogicalDevice();
}

void VulkanDevice::Shutdown() {
    DestroyLogicalDevice();

    DestroySurface();

    if (bValidationEnabled)
        DestroyDebugger();

    DestroyInstance();
}

void VulkanDevice::CreateInstance() {
    constexpr VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "BrioApp",
        .applicationVersion = VK_MAKE_VERSION(0,1,0),
        .pEngineName = "Brio Engine",
        .engineVersion = VK_MAKE_VERSION(0,1,0),
        .apiVersion = VK_API_VERSION_1_3
    };

    TArray<char const*> layers = {};
    if (bValidationEnabled)
        layers.Add("VK_LAYER_KHRONOS_validation");

    TArray<char const*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
    if (bValidationEnabled)
        extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#if PLATFORM_LINUX
    extensions.Add(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif PLATFORM_WINDOWS
    extensions.Add("VK_KHR_win32_surface");
#elif PLATFORM_MACOS
    extensions.Add("VK_KHR_metal_surface");
    extensions.Add(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.Add(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

    const VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if PLATFORM_MACOS
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
#endif
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(layers.Num()),
        .ppEnabledLayerNames = layers.Data(),
        .enabledExtensionCount = static_cast<u32>(extensions.Num()),
        .ppEnabledExtensionNames = extensions.Data(),
    };

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    VK_CHECK(result);
}

void VulkanDevice::CreateDebugger() {
    constexpr VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugCallback,
        .pUserData = nullptr,
    };

    VkResult result = INSTANCE_FUNCTION(vkCreateDebugUtilsMessengerEXT)(instance, &createInfo, nullptr, &debugger);
    VK_CHECK(result);
}

void VulkanDevice::CreateSurface(FNativeWindowHandle const &windowHandle) {
#if PLATFORM_LINUX
    const VkWaylandSurfaceCreateInfoKHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .display = static_cast<wl_display*>(windowHandle.wayland.wldisplay),
        .surface = static_cast<wl_surface*>(windowHandle.wayland.wlsurface),
    };
    VkResult result = vkCreateWaylandSurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
    VK_CHECK(result);
#endif
}

void VulkanDevice::PickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        LOG_FATAL(LogRHI, "No compatible GPUs found!");
    }

    TArray<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.Data());

    VkPhysicalDevice fallback {};
    for (VkPhysicalDevice device : devices) {
        if (IsDeviceSuitable(device) && FindQueueFamilies(device)) {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(device, &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                physicalDevice = device;
                vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
                LOG_INFO(LogRHI, "Selected GPU: {}", properties.deviceName);
                return;
            }

            fallback = device;
        }
    }

    if (fallback) {
        physicalDevice = fallback;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(fallback, &properties);
        vkGetPhysicalDeviceMemoryProperties(fallback, &memoryProperties);
        LOG_INFO(LogRHI, "Selected GPU: {}", properties.deviceName);
        return;
    }

    LOG_FATAL(LogRHI, "Found {} GPUs but none met the requirements.", deviceCount);
}

void VulkanDevice::CreateLogicalDevice() {
    std::unordered_set<u32> uniqueFamilies;
    uniqueFamilies.insert(graphicsFamilyIndex);
    uniqueFamilies.insert(presentFamilyIndex);

    f32 priority = 1.0f;
    TArray<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (u32 family : uniqueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = family,
            .queueCount = 1,
            .pQueuePriorities = &priority,
        };
        queueCreateInfos.Add(queueCreateInfo);
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

    TArray<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#if PLATFORM_MACOS
    extensions.Add(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features2,
        .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.Num()),
        .pQueueCreateInfos = queueCreateInfos.Data(),
        .enabledExtensionCount = static_cast<u32>(extensions.Num()),
        .ppEnabledExtensionNames = extensions.Data(),
    };

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
    VK_CHECK(result);

    vkGetDeviceQueue(logicalDevice, graphicsFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, presentFamilyIndex, 0, &presentQueue);
}

void VulkanDevice::DestroyInstance() {
    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
}

void VulkanDevice::DestroyDebugger() {
    INSTANCE_FUNCTION(vkDestroyDebugUtilsMessengerEXT)(instance, debugger, nullptr);
    debugger = nullptr;
}

void VulkanDevice::DestroySurface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = nullptr;
}

void VulkanDevice::DestroyLogicalDevice() {
    vkDestroyDevice(logicalDevice, nullptr);
    logicalDevice = nullptr;
}

bool VulkanDevice::CheckValidationSupport() {
    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    TArray<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

    for (auto const& layer : availableLayers) {
        if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
            LOG_INFO(LogRHI, "System supports validation layers.");
            return true;
        }
    }

    LOG_WARNING(LogRHI, "Validation was requested by application, but the system does not support validation layers!");
    return false;
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice) {
    u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    TArray<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.Data());

    bool bSupportsSwapchain = false;
    bool bSupportsPortabilitySubset = true;
#if PLATFORM_MACOS
    // macOS requires the portability subset for MoltenVK, so we actually need to check for its support.
    bSupportsPortabilitySubset = false;
#endif

    for (auto const& extension : availableExtensions) {
        if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            bSupportsSwapchain = true;
        }
#if PLATFORM_MACOS
        else if (strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0) {
            bSupportsPortabilitySubset = true;
        }
#endif

        if (bSupportsSwapchain && bSupportsPortabilitySubset) {
            return true;
        }
    }

    return false;
}

bool VulkanDevice::FindQueueFamilies(VkPhysicalDevice device) {
    bool bHasGraphics = false;
    bool bHasPresentation = false;

    u32 familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

    TArray<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.Data());

    for (u32 i = 0; i < familyCount; i++) {
        if (!bHasGraphics && families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamilyIndex = i;
            bHasGraphics = true;
        }

        VkBool32 bCanPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &bCanPresent);
        if (!bHasPresentation && bCanPresent) {
            presentFamilyIndex = i;
            bHasPresentation = true;
        }
    }

    return bHasGraphics && bHasPresentation;
}
