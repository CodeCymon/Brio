// Copyright (c) Simon Kirsch 2026.

#pragma once

struct UIntPoint;
struct NativeWindowData;
class IDynamicRHI;

extern IDynamicRHI* GDynamicRHI;

namespace RHI {
    bool Create(NativeWindowData const& windowData, UIntPoint const& initialExtent);
    void Destroy();
}