#pragma once
#include <vulkan/vulkan_core.h>

#define INSTANCE_FUNCTION(function) \
    reinterpret_cast<PFN_##function>(vkGetInstanceProcAddr(instance, #function))

