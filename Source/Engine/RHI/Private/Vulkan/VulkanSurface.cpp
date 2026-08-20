// Copyright (c) Simon Kirsch 2026.

#include "VulkanSurface.h"

#include "Window.h"
#include "Vulkan/VulkanCheck.h"
#include "Vulkan/VulkanInstance.h"

#if PLATFORM_LINUX
#include <vulkan/vulkan_wayland.h>
#endif

void VulkanSurface::Initialize(VulkanInstance const* inInstance, NativeWindowData const &windowData) {
    instance = inInstance;

#if PLATFORM_LINUX
    const VkWaylandSurfaceCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .display = static_cast<wl_display*>(windowData.wayland.wldisplay),
        .surface = static_cast<wl_surface*>(windowData.wayland.wlsurface),
    };
    VkResult result = vkCreateWaylandSurfaceKHR(instance->Instance(), &createInfo, nullptr, &surface);
    VK_CHECK(result);
#else
#pragma error("Not implemented for this platform")
#endif
}

void VulkanSurface::Shutdown() {
    vkDestroySurfaceKHR(instance->Instance(), surface, nullptr);
    surface = nullptr;
}
