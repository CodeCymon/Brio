#include "Platform.h"

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



bool Platform::Initialize(FConfig const &Config) {

    if (glfwInit() != GLFW_TRUE)
        return false;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, Config.resizable ? GLFW_TRUE : GLFW_FALSE);

    window = glfwCreateWindow(Config.width, Config.height, Config.title, nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        auto* ptr = static_cast<Platform*>(glfwGetWindowUserPointer(window));
        ptr->OnResizeDelegate.Broadcast(static_cast<u32>(width), static_cast<u32>(height));
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        auto* ptr = static_cast<Platform*>(glfwGetWindowUserPointer(window));
        ptr->OnCloseDelegate.Broadcast();
    });

    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focus) {
        auto* ptr = static_cast<Platform*>(glfwGetWindowUserPointer(window));
        ptr->OnFocusChangeDelegate.Broadcast(focus == GLFW_TRUE);
    });

    return true;
}

void Platform::Shutdown() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Platform::PollEvents() {
    glfwPollEvents();
}

FNativeWindowHandle Platform::NativeHandle() const {
    FNativeWindowHandle handle {};
#if PLATFORM_LINUX
    handle.wayland.wldisplay = glfwGetWaylandDisplay();
    handle.wayland.wlsurface = glfwGetWaylandWindow(window);
#endif
    return handle;
}
