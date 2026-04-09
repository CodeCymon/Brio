#include "Device.h"

#include <cstring>
#include <unordered_set>

#include "VulkanCheck.h"
#include "Core/Asserts/Assert.h"

#if PLATFORM_MACOS
#include <vulkan/vulkan_beta.h>
#endif

#define INSTANCE_FUNCTION(function) \
    reinterpret_cast<PFN_##function>(vkGetInstanceProcAddr(instance_, #function))

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

void VulkanDevice::init(Config const &config) {
    if (config.validation && checkValidationSupport()) {
        use_validation_ = true;
    }

    createInstance(config);

    if (use_validation_) {
        createDebugger();
    }

    createSurface(config);

    pickPhysicalDevice();
    createDevice();

    LOG_INFO(LogRHI, "Device initialized.");
}

void VulkanDevice::shutdown() {
    destroyDevice();

    destroySurface();

    if (use_validation_) {
        destroyDebugger();
    }

    destroyInstance();

    LOG_INFO(LogRHI, "Device shutdown.");
}

void VulkanDevice::createInstance(Config const &config) {
    constexpr VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "BrioApp",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "BrioEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_4
    };

    TArray<const char*> layers = {};
    if (use_validation_) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    TArray<const char*> extensions = config.platformExtensionsFn();
    ASSERT(!extensions.empty(), "platform must provide at minimum the surface extensions!");
    if (use_validation_) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#if PLATFORM_MACOS
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

    const VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if PLATFORM_MACOS
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<u32>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
    VK_CHECK(result, "Instance creation failed!");
    LOG_DETAIL(LogRHI, "Instance created.");
}

void VulkanDevice::createDebugger() {
    constexpr VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    VkResult result = INSTANCE_FUNCTION(vkCreateDebugUtilsMessengerEXT)(instance_, &createInfo, nullptr, &debugger_);
    VK_CHECK(result, "Debugger creation failed!");
    LOG_DETAIL(LogRHI, "Debugger created.");
}

void VulkanDevice::createSurface(Config const &config) {
    surface_ = static_cast<VkSurfaceKHR>(config.platformSurfaceFn(instance_));
    ASSERT(surface_, "Surface creation failed!");
    LOG_DETAIL(LogRHI, "Surface created.");
}

void VulkanDevice::pickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

    if (deviceCount == 0) {
        LOG_FATAL(LogRHI, "No compatible GPUs found!");
        return;
    }

    TArray<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

    VkPhysicalDevice fallback{};
    for (VkPhysicalDevice device : devices) {
        if (isDeviceSuitable(device) && findQueueFamilies(device)) {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(device, &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                physical_device_ = device;
                vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties_);
                LOG_INFO(LogRHI, "Selected GPU: {}", properties.deviceName);
                return;
            }

            fallback = device;
        }
    }

    if (fallback) {
        physical_device_ = fallback;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(fallback, &properties);
        vkGetPhysicalDeviceMemoryProperties(fallback, &memory_properties_);
        LOG_INFO(LogRHI, "Selected GPU: {}", properties.deviceName);
        return;
    }

    LOG_FATAL(LogRHI, "Found {} GPUs but none met the requirements.", deviceCount);
}

void VulkanDevice::createDevice() {
    ASSERT(physical_device_, "physical device must be valid!");

    std::unordered_set<u32> uniqueFamilies;
    uniqueFamilies.insert(graphics_family_);
    uniqueFamilies.insert(present_family_);

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

    TArray<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#if PLATFORM_MACOS
    extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features2,
        .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<u32>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkResult result = vkCreateDevice(physical_device_, &createInfo, nullptr, &device_);
    VK_CHECK(result, "Device creation failed!");
    LOG_DETAIL(LogRHI, "Device created.");

    vkGetDeviceQueue(device_, graphics_family_, 0, &graphics_queue_);
    LOG_DETAIL(LogRHI, "acquired graphics queue.");
    vkGetDeviceQueue(device_, present_family_, 0, &present_queue_);
    LOG_DETAIL(LogRHI, "acquired present queue.");
}

void VulkanDevice::destroyInstance() {
    vkDestroyInstance(instance_, nullptr);
    instance_ = nullptr;
    LOG_DETAIL(LogRHI, "Instance destroyed.");
}

void VulkanDevice::destroyDebugger() {
    INSTANCE_FUNCTION(vkDestroyDebugUtilsMessengerEXT)(instance_, debugger_, nullptr);
    debugger_ = nullptr;
    LOG_DETAIL(LogRHI, "Debugger destroyed.");
}

void VulkanDevice::destroySurface() {
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    surface_ = nullptr;
    LOG_DETAIL(LogRHI, "Surface destroyed.");
}

void VulkanDevice::destroyDevice() {
    vkDestroyDevice(device_, nullptr);
    device_ = nullptr;
    LOG_DETAIL(LogRHI, "Device destroyed.");
}

bool VulkanDevice::checkValidationSupport() {
    u32 count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    TArray<VkLayerProperties> properties(count);
    vkEnumerateInstanceLayerProperties(&count, properties.data());

    for (auto const& property : properties) {
        if (strcmp(property.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
            LOG_DEBUG(LogRHI, "System supports validation layers.");
            return true;
        }
    }

    LOG_WARN(LogRHI, "Validation was requested by application, but system does not support validation layers!");
    return false;
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice physical_device) {
    ASSERT(physical_device, "physical device must be valid!");

    u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensionCount, nullptr);

    TArray<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensionCount, extensions.data());

    bool supportsSwapchain = false;
    bool supportsPortabilitySubset = false;

#if not PLATFORM_MACOS
    // portability subset is only required on MacOS because of MoltenVK
    supportsPortabilitySubset = true;
#endif

    for (auto const& extension : extensions) {
        if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            supportsSwapchain = true;
        }
#if PLATFORM_MACOS
        else if (strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0) {
            supportsPortabilitySubset = true;
        }
#endif

        if (supportsSwapchain && supportsPortabilitySubset) {
            return true;
        }
    }

    return false;
}

bool VulkanDevice::findQueueFamilies(VkPhysicalDevice physical_device) {
    ASSERT(physical_device, "physical device must be valid!");
    ASSERT(surface_, "surface must be valid!");

    bool hasGraphics = false;
    bool hasPresent = false;

    u32 familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &familyCount, nullptr);

    TArray<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &familyCount, families.data());

    for (u32 i = 0; i < familyCount; i++) {
        if (!hasGraphics && families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family_ = i;
            hasGraphics = true;
        }

        VkBool32 canPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface_, &canPresent);
        if (!hasPresent && canPresent == VK_TRUE) {
            present_family_ = i;
            hasPresent = true;
        }
    }

    return hasGraphics && hasPresent;
}
