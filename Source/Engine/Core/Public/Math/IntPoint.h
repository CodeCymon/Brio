// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"

struct IntPoint {
    i32 x, y;
    IntPoint() : x(0), y(0) {}
    IntPoint(i32 _x, i32 _y) : x(_x), y(_y) {}
};

struct UIntPoint {
    u32 x, y;
    UIntPoint() : x(0), y(0) {}
    UIntPoint(u32 _x, u32 _y) : x(_x), y(_y) {}
};