#include "Platform.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Core/Asserts/Assert.h"

// TODO: replace with event system
struct ResizeEventData {
    bool resized {false};
    u32 width {0};
    u32 height {0};
};

struct Platform::Impl {
    GLFWwindow *window{nullptr};
    ResizeEventData resize_event{}; // TODO: event system
};

Platform::Platform() {
    data = new Impl;
}

Platform::~Platform() {
    delete data;
}

void Platform::init(Config const &config) {
    if (!glfwInit()) {
        LOG_FATAL(LogPlatform, "Failed to initialize GLFW!");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, config.resizable == true ? GLFW_TRUE : GLFW_FALSE);
    data->window = glfwCreateWindow(
        static_cast<i32>(config.width),
        static_cast<i32>(config.height),
        config.title,
        nullptr, nullptr);
    ASSERT(data->window, "Window creation failed!");

    glfwSetWindowUserPointer(data->window, data);
    glfwSetFramebufferSizeCallback(data->window, [](GLFWwindow *window, int width, int height) {
        Impl* ptr = static_cast<Impl *>(glfwGetWindowUserPointer(window));
        if (ptr) {
            ptr->resize_event.resized = true;
            ptr->resize_event.width = width;
            ptr->resize_event.height = height;
        }
    });
}

void Platform::shutdown() {
    glfwDestroyWindow(data->window);
    glfwTerminate();
}

void Platform::pollEvents() {
    glfwPollEvents();
}

bool Platform::shouldClose() const {
    return glfwWindowShouldClose(data->window);
}

// TODO: event system replace
bool Platform::resizeEvent(u32 *width, u32 *height) {
    if (data->resize_event.resized == false) {
        *width = 0;
        *height = 0;
        return false;
    }

    *width = data->resize_event.width;
    *height = data->resize_event.height;
    data->resize_event.resized = false; // handled event
    return true;
}

void* Platform::getSurface(void *instance) const {
    VkSurfaceKHR surface {};
    glfwCreateWindowSurface(
        static_cast<VkInstance>(instance),
        data->window,
        nullptr,
        &surface
  );
    return static_cast<void*>(surface);
}

TArray<const char *> Platform::getExtensions() {
    u32 extensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    TArray<const char *> extensions(glfwExtensions, glfwExtensions + extensionCount);
    return extensions;
}
