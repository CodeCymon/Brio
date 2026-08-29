// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vk_enum_string_helper.h>

#include "CoreMinimal.h"

#include "LogVulkan.h"

#if BUILD_DEBUG
    #define VK_CHECK(result) \
        do { \
            if (result != VK_SUCCESS) { \
                LOG_FATAL(LogVulkan, "Vulkan Check failed:! [{}:{}]: Error: {}", __FILE__, __LINE__, string_VkResult(result)); \
                DEBUG_BREAK(); \
            } \
        } while (false);
#else
    #define VK_CHECK(result)
#endif
