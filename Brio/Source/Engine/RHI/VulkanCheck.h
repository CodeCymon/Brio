#pragma once

#include "Core/Asserts/Assert.h"

#include <vulkan/vk_enum_string_helper.h>

#define VK_CHECK(result, msg) ASSERT(result == VK_SUCCESS, "{} [Error: {}]", msg, string_VkResult(result));