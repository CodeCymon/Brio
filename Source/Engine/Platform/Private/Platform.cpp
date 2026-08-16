// Copyright (c) Simon Kirsch 2026.

#include "Platform.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

DEFINE_LOG_CATEGORY(LogPlatform)

bool Platform::Initialize() {
    if (glfwInit() != GLFW_TRUE) {
        LOG_ERROR(LogPlatform, "Failed to initialize GLFW");
        return false;
    }
    return true;
}

void Platform::Shutdown() {
    glfwTerminate();
}

void Platform::PollEvents() {
    glfwPollEvents();
}
