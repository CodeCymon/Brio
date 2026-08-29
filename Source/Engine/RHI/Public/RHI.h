// Copyright (c) Simon Kirsch 2026.

#pragma once

struct NativeWindowData;
class IDynamicRHI;

extern IDynamicRHI* GDynamicRHI;

namespace RHI {
    void Create(NativeWindowData const& windowData);
    void Destroy();
}