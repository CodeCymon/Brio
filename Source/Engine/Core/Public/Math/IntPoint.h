// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"

struct IntPoint;
struct UIntPoint;
struct IntVec;
struct UIntVec;

struct IntPoint {
    i32 x, y;
    IntPoint() : x(0), y(0) {}
    IntPoint(i32 _x, i32 _y) : x(_x), y(_y) {}
};

struct UIntPoint {
    u32 x, y;
    UIntPoint() : x(0), y(0) {}
    UIntPoint(u32 _x, u32 _y) : x(_x), y(_y) {}
    UIntPoint(UIntVec const& v);
};

struct IntVec {
    i32 x, y, z;
    IntVec() : x(0), y(0), z(0) {}
    IntVec(i32 _x, i32 _y, i32 _z) : x(_x), y(_y), z(_z) {}
};

struct UIntVec {
    u32 x, y, z;
    UIntVec() : x(0), y(0), z(0) {}
    UIntVec(u32 _x, u32 _y, u32 _z) : x(_x), y(_y), z(_z) {}
};

inline UIntPoint::UIntPoint(UIntVec const &v) {
    x = v.x;
    y = v.y;
}