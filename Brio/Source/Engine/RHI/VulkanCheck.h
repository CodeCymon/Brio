#pragma once

#include "Core/Asserts/Assert.h"

#include <vulkan/vk_enum_string_helper.h>

#define VK_CHECK(result) ASSERT(result == VK_SUCCESS, "Vulkan Action failed: {}", string_VkResult(result));