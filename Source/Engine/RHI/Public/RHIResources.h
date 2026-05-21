#pragma once

#include "CoreMinimal.h"

struct FRHIFrameContext {
    struct FRHITexture {} swapchainImage;
    u32 frameIndex;
};
