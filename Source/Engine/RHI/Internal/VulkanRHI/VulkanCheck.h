#pragma once

#include <vulkan/vk_enum_string_helper.h>

#include "Log/Assert.h"

#if BUILD_DEBUG
    #define VK_CHECK(result) \
        do { \
            if (result != VK_SUCCESS) { \
                LOG_FATAL(LogAssert, "Vulkan Check failed! [{}:{}]: Error: {}", __FILE__, __LINE__, string_VkResult(result)); \
                DEBUG_BREAK(); \
            } \
        } while (0);
#else
    #define VK_CHECK(result)
#endif