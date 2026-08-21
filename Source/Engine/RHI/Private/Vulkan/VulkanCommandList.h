// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <vulkan/vulkan_core.h>

#include "CommandList.h"

class VulkanCommandList : public ICommandList {
public:
    void BeginDebugLabel(char const* label) override;

    void EndDebugLabel() override;

    void InsertDebugLabel(char const* label) override;

public:
    void BindActiveCommandBuffer(VkCommandBuffer commandBuffer);

private:
    VkCommandBuffer cmd {};
};