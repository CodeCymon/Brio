// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <vulkan/vulkan_core.h>

struct VulkanLoader {
    PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT {};
    PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT {};
    PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT {};

    void LoadFunctions(VkDevice device);
};
