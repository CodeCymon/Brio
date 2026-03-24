#include "Platform.h"
#include "Core/Log/Logger.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void Platform::initialize() {
  glfwInit();
  LOG_INFO(LogPlatform, "Initialization complete.");
}

void Platform::shutdown() {
  glfwTerminate();
  LOG_INFO(LogPlatform, "Shutdown complete.");
}

void Platform::update() {}

bool Platform::closeRequested() const { return true; }
