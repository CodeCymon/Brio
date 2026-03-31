#include "Platform.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Core/Asserts/Assert.h"

struct Platform::Impl {
    GLFWwindow *window{nullptr};
};

Platform::Platform() {
    data = new Impl;
}

Platform::~Platform() {
    delete data;
}

void Platform::init(Config const &config) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, config.resizable);
    data->window = glfwCreateWindow(
        static_cast<i32>(config.width),
        static_cast<i32>(config.height),
        config.title,
        nullptr, nullptr);
    ASSERT(data->window, "Window creation failed!");
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

void* Platform::getSurface(void *instance) {
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
