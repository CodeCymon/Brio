// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan.h>

#include "Core/CoreMacros.h"

class VulkanInstance {
public:
    VulkanInstance() = default;
    ~VulkanInstance() = default;

    NON_COPYABLE(VulkanInstance);
    NON_MOVEABLE(VulkanInstance);

public:
    void Initialize(bool bEnableValidation);
    void Shutdown();

public:
    [[nodiscard]] bool IsValidationEnabled() const {return bValidationEnabled; }

    [[nodiscard]] VkInstance Instance() const { return instance; }
    [[nodiscard]] VkDebugUtilsMessengerEXT DebugMessenger() const { return debugMessenger; }

private:
    void CreateInstance();
    void CreateDebugMessenger();

    void DestroyInstance();
    void DestroyDebugMessenger();

    static bool HasValidationLayerSupport();

private:
    VkInstance instance {};
    VkDebugUtilsMessengerEXT debugMessenger {};

    bool bValidationEnabled {false};
};
