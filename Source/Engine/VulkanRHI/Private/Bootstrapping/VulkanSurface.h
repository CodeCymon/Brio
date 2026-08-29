// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan.h>

#include "Core/CoreMacros.h"


class VulkanInstance;
struct NativeWindowData;

class VulkanSurface {
public:
    VulkanSurface() = default;
    ~VulkanSurface() = default;

    NON_COPYABLE(VulkanSurface);
    NON_MOVEABLE(VulkanSurface);

    void Initialize(VulkanInstance const* inInstance, NativeWindowData const& windowData);
    void Shutdown();

    [[nodiscard]] VkSurfaceKHR Surface() const { return surface; }

private:
    VkSurfaceKHR surface {};

    VulkanInstance const* instance {nullptr};
};
