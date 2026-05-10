#pragma once

#include "Types/CoreTypes.h"

#include <vulkan/vulkan_core.h>

struct FNativeWindowHandle;

class VulkanDevice {
public:
    struct FConfig {
        FNativeWindowHandle const& windowHandle;
        bool bValidationEnabled;
    };

    VulkanDevice() = default;
    ~VulkanDevice() = default;

    VulkanDevice(VulkanDevice const &) = delete;
    VulkanDevice(VulkanDevice &&) = delete;
    VulkanDevice& operator=(VulkanDevice const &) = delete;
    VulkanDevice& operator=(VulkanDevice &&) = delete;

    void Initialize(FConfig const& config);
    void Shutdown();

    [[nodiscard]] VkInstance Instance() const { return instance; }
    [[nodiscard]] VkSurfaceKHR Surface() const { return surface; }
    [[nodiscard]] VkPhysicalDevice PhysicalDevice() const { return physicalDevice; }
    [[nodiscard]] VkDevice LogicalDevice() const { return logicalDevice; }
    [[nodiscard]] VkQueue GraphicsQueue() const { return graphicsQueue; }
    [[nodiscard]] VkQueue PresentQueue() const { return presentQueue; }
    [[nodiscard]] u32 GraphicsFamilyIndex() const { return graphicsFamilyIndex; }
    [[nodiscard]] u32 PresentFamilyIndex() const { return presentFamilyIndex; }
    [[nodiscard]] VkPhysicalDeviceMemoryProperties const &MemoryProperties() const { return memoryProperties; }

private:
    void CreateInstance();
    void CreateDebugger();
    void CreateSurface(FNativeWindowHandle const& windowHandle);
    void PickPhysicalDevice();
    void CreateLogicalDevice();

    void DestroyInstance();
    void DestroyDebugger();
    void DestroySurface();
    void DestroyLogicalDevice();

    static bool CheckValidationSupport();
    static bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);
    bool        FindQueueFamilies(VkPhysicalDevice device);

private:
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugger{};
    VkSurfaceKHR surface{};
    VkPhysicalDevice physicalDevice {};
    VkPhysicalDeviceMemoryProperties memoryProperties {};
    VkDevice logicalDevice {};
    VkQueue graphicsQueue {};
    VkQueue presentQueue {};
    u32 graphicsFamilyIndex {};
    u32 presentFamilyIndex {};
    bool bValidationEnabled {false};
};
