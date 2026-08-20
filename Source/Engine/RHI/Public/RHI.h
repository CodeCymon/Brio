// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "RHIAPI.h"

struct NativeWindowData;
class IDynamicRHI;

extern RHI_API IDynamicRHI* GDynamicRHI;

namespace RHI {
    RHI_API void Create(NativeWindowData const& windowData);
    RHI_API void Destroy();
}