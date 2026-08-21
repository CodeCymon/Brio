// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "CoreMinimal.h"

class ICommandList;

struct RHIFrameContext {
    ICommandList* cmdList {nullptr};
    // RHITextureRef swapchainTexture {};
    u32 frameIndex {0};
};
