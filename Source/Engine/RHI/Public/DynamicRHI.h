#pragma once

#include "RHIAPI.h"
#include "CoreMinimal.h"

#include "RHIResources.h"


struct FNativeWindowHandle;

constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

class RHI_API IDynamicRHI {
public:
    virtual ~IDynamicRHI() = default;

    virtual void Initialize(FNativeWindowHandle const &windowHandle) = 0;
    virtual void Shutdown() = 0;

    virtual void OnResize(u32 width, u32 height) = 0;

    virtual FFrameContext BeginFrame() = 0;
    virtual void EndFrame() = 0;
};

extern RHI_API IDynamicRHI* GDynamicRHI;

namespace RHI {
    RHI_API void Create(FNativeWindowHandle const& windowHandle);
    RHI_API void Destroy();

    RHI_API IDynamicRHI& Get();
}