#pragma once
#include <functional>
#include <vulkan/vulkan_core.h>

#include "Common/Types.h"
#include "Core/Containers/Array.h"

class VulkanDevice {
public:
    struct Config {
        std::function<void *(void *)> platformSurfaceFn;
        std::function<TArray<const char*>()> platformExtensionsFn;
        bool validation { false };
    };

    VulkanDevice() = default;
    ~VulkanDevice() = default;

    VulkanDevice(VulkanDevice const &) = delete;
    VulkanDevice &operator=(VulkanDevice const &) = delete;

    void init(Config const &config);
    void shutdown();

    [[nodiscard]] VkInstance        instance()          const { return instance_; }
    [[nodiscard]] VkSurfaceKHR      surface()           const { return surface_; }
    [[nodiscard]] VkPhysicalDevice  physicalDevice()    const { return physical_device_; }
    [[nodiscard]] VkDevice          device()            const { return device_; }
    [[nodiscard]] VkQueue           graphicsQueue()     const { return graphics_queue_; }
    [[nodiscard]] VkQueue           presentQueue()      const { return present_queue_; }
    [[nodiscard]] u32               graphicsFamily()    const { return graphics_family_; }
    [[nodiscard]] u32               presentFamily()     const { return present_family_; }
    [[nodiscard]] VkPhysicalDeviceMemoryProperties const& memoryProperties() const { return memory_properties_; }

private:
    void createInstance(Config const &config);
    void createDebugger();
    void createSurface(Config const &config);
    void pickPhysicalDevice();
    void createDevice();

    void destroyInstance();
    void destroyDebugger();
    void destroySurface();
    void destroyDevice();

    static bool checkValidationSupport();
    static bool isDeviceSuitable(VkPhysicalDevice physical_device);
    bool        findQueueFamilies(VkPhysicalDevice physical_device);

private:
    VkInstance instance_{};
    VkDebugUtilsMessengerEXT debugger_{};
    VkSurfaceKHR surface_{};
    VkPhysicalDevice physical_device_{};
    VkPhysicalDeviceMemoryProperties memory_properties_{};
    VkDevice device_{};
    VkQueue graphics_queue_{};
    VkQueue present_queue_{};
    u32 graphics_family_{};
    u32 present_family_{};
    bool use_validation_ { false };
};