#pragma once
#include <functional>
#include <vulkan/vulkan_core.h>

#include "Common/Types.h"
#include "Core/Containers/Array.h"

class VulkanDevice
{
public:
    using getPlatformSurfaceFn = std::function<void*(void*)>;
    using getPlatformExtensionsFn = TArray<const char*> (*)();

    struct Config {
        getPlatformExtensionsFn getExtensions;
        getPlatformSurfaceFn getSurface;
        bool useValidation;
    };

    struct GPU {
        VkPhysicalDevice handle{};
        VkPhysicalDeviceMemoryProperties memoryProperties{};

        [[nodiscard]] bool supportsSwapchain() const;
    };

    struct Queue {
        VkQueue handle{};
        u32 familyIndex{};
        bool hasFamilyIndex { false };
    };

    VulkanDevice();
    ~VulkanDevice();

    VulkanDevice(VulkanDevice const &) = delete;
    VulkanDevice &operator=(VulkanDevice const &) = delete;

    void init(Config const &config);
    void shutdown();

private:
    void createInstance(getPlatformExtensionsFn const &getPlatformExtensions, bool useValidation);
    void destroyInstance();
    static TArray<const char*> getInstanceExtensions(getPlatformExtensionsFn const &getPlatformExtensions, bool useValidation);
    static TArray<const char*> getInstanceLayers(bool useValidation);

    void createDebugger();
    void destroyDebugger();

    void createSurface(getPlatformSurfaceFn const &getSurface);
    void destroySurface();

    void pickGPU();
    void queryQueueFamilies(VkPhysicalDevice physicalDevice);

    void createDevice();
    void destroyDevice();
    static TArray<const char*> getDeviceExtensions();

private:
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugger{};

    VkSurfaceKHR surface{};

    GPU gpu{};
    VkDevice device{};
    Queue graphicsQueue{};
    Queue presentQueue{};
};
