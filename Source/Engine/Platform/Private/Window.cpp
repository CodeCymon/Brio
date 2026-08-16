// Copyright (c) Simon Kirsch 2026.

#include "Window.h"

#include "Platform.h"
#include "Log/Log.h"

#if PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#if PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

bool Window::Create(Config const &config, Window &window) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);

    window.handle = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
    if (!window.handle) {
        LOG_ERROR(LogPlatform, "Failed to create GLFW window");
        return false;
    }

    glfwSetWindowUserPointer(window.handle, &window);

    glfwSetWindowCloseCallback(window.handle, [] (GLFWwindow* window) {
        auto* ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (ptr->OnCloseDelegate.IsBound())
            ptr->OnCloseDelegate.Execute();
        ptr->Close();
    });

    return true;
}

void Window::Close() {
    glfwDestroyWindow(handle);
    handle = nullptr;
}

NativeWindowData Window::NativeData() const {
    NativeWindowData data{};
#if PLATFORM_WINDOWS

#elif PLATFORM_LINUX
    data.wayland.wldisplay = glfwGetWaylandDisplay();
    data.wayland.wlsurface = glfwGetWaylandWindow(handle);
#elif PLATFORM_MACOS

#endif
    return data;
}
