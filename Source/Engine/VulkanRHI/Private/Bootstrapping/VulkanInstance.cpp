// Copyright (c) Simon Kirsch 2026.

#include "VulkanInstance.h"

#include <cstring>

#include "VulkanCheck.h"
#include "Containers/Array.h"

#if PLATFORM_LINUX
#include <vulkan/vulkan_wayland.h>
#elif PLATFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#elif PLATFORM_MACOS
#include <vulkan/vulkan_metal.h>
#endif

#define INSTANCE_FN(function) reinterpret_cast<PFN_##function>(vkGetInstanceProcAddr(instance, #function))

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
                                                      void* pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOG_ERROR(LogVulkan, "{} - {}: {}", pCallbackData->messageIdNumber,
                  pCallbackData->pMessageIdName, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_WARNING(LogVulkan, "{} - {}: {}", pCallbackData->messageIdNumber,
                    pCallbackData->pMessageIdName, pCallbackData->pMessage);
    } else {
        LOG_INFO(LogVulkan, "{} - {}: {}", pCallbackData->messageIdNumber,
                 pCallbackData->pMessageIdName, pCallbackData->pMessage);
    }

    return VK_FALSE;
}

void VulkanInstance::Initialize(bool bEnableValidation) {
    bValidationEnabled = bEnableValidation && HasValidationLayerSupport();

    CreateInstance();

    if (bValidationEnabled)
        CreateDebugMessenger();
}

void VulkanInstance::Shutdown() {
    if (bValidationEnabled)
        DestroyDebugMessenger();

    DestroyInstance();
}

void VulkanInstance::CreateInstance() {
    constexpr VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "BrioEditor",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "BrioEngine",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_4
    };

    Array<const char*> layers = {};
    if (bValidationEnabled) {
        layers.Add("VK_LAYER_KHRONOS_validation");
    }

    Array<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
    if (bValidationEnabled) {
        extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#if PLATFORM_LINUX
    extensions.Add(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif PLATFORM_WINDOWS
    extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif PLATFORM_MACOS
    extensions.Add(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
    extensions.Add(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.Add(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

    const VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
#if PLATFORM_MACOS
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(layers.Size()),
        .ppEnabledLayerNames = layers.Data(),
        .enabledExtensionCount = static_cast<u32>(extensions.Size()),
        .ppEnabledExtensionNames = extensions.Data(),
    };

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    VK_CHECK(result);
}

void VulkanInstance::CreateDebugMessenger() {
    constexpr VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugMessengerCallback,
        .pUserData = nullptr,
    };

    VkResult result = INSTANCE_FN(vkCreateDebugUtilsMessengerEXT)(
        instance, &createInfo, nullptr, &debugMessenger);
    VK_CHECK(result);
}

void VulkanInstance::DestroyInstance() {
    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
}

void VulkanInstance::DestroyDebugMessenger() {
    INSTANCE_FN(vkDestroyDebugUtilsMessengerEXT)(instance, debugMessenger, nullptr);
    debugMessenger = nullptr;
}


bool VulkanInstance::HasValidationLayerSupport() {
    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    Array<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

    for (auto const& layer : availableLayers) {
        if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
            LOG_INFO(LogVulkan, "System supports requested validation layers.");
            return true;
        }
    }

    LOG_WARNING(
        LogVulkan,
        "Validation layers were requested by application, but the system does not support requested validation layers!");
    return false;
}
