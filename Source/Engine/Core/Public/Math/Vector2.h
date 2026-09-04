// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"
#include "Log/Assert.h"
#include "Math/Math.h"
#include "Math/IntPoint.h"

// 2-Component floating point vector
struct CORE_API Vec2 {
    f32 x, y;

public:
    constexpr Vec2() : x(0), y(0) {}

    constexpr explicit Vec2(f32 s) : x(s), y(s) {}

    constexpr Vec2(f32 x_, f32 y_) : x(x_), y(y_) {}

    constexpr Vec2(UIntPoint const& p) : x(p.x), y(p.y) {}

    constexpr Vec2(IntPoint const& p) : x(p.x), y(p.y) {}

    constexpr Vec2(IntVec const& p) : x(p.x), y(p.y) {}

    constexpr Vec2(UIntVec const& p) : x(p.x), y(p.y) {}

public:
    constexpr f32& operator[](i32 i) {
        ASSERT(i >= 0 && i < 2);
        return *(&x + i);
    }

    constexpr f32 operator[](i32 i) const {
        ASSERT(i >= 0 && i < 2);
        return *(&x + i);
    }

    constexpr Vec2 operator+(Vec2 const& v) const {
        return {x + v.x, y + v.y};
    }

    constexpr Vec2 operator-(Vec2 const& v) const {
        return {x - v.x, y - v.y};
    }

    constexpr Vec2 operator*(Vec2 const& v) const {
        return {x * v.x, y * v.y};
    }

    constexpr Vec2 operator*(f32 s) const {
        return {x * s, y * s};
    }

    constexpr Vec2 operator/(f32 s) const {
        f32 const invS = 1.0f / s;
        return {x * invS, y * invS};
    }

    constexpr Vec2 operator-() const {
        return {-x, -y};
    }


    Vec2& operator+=(Vec2 const& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2& operator-=(Vec2 const& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2& operator*=(Vec2 const& v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    Vec2& operator*=(f32 s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vec2& operator/=(f32 s) {
        f32 const invS = 1.0f / s;
        x *= invS;
        y *= invS;
        return *this;
    }

public:
    constexpr bool operator==(Vec2 const& v) const {
        return x == v.x && y == v.y;
    }

    constexpr bool operator!=(Vec2 const& v) const {
        return !(*this == v);
    }

public:
    [[nodiscard]] constexpr f32 LengthSquared() const {  return x * x + y * y; }
    [[nodiscard]] constexpr f32 Length() const {  return Math::Sqrt(LengthSquared()); }

    [[nodiscard]] Vec2 Normalized() const {
        f32 const lenSquared = LengthSquared();
        ASSERT(lenSquared > Math::kEpsilonF32 * Math::kEpsilonF32);
        f32 const invLen = 1.0f / Math::Sqrt(lenSquared);
        return {x * invLen, y * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

public:
    static constexpr f32 Dot(Vec2 const& a, Vec2 const& b) {
        return a.x * b.x + a.y * b.y;
    }

    static constexpr f32 Distance(Vec2 const& a, Vec2 const& b) {
        return (b - a).Length();
    }

    static constexpr f32 DistanceSquared(Vec2 const& a, Vec2 const& b) {
        return (b - a).LengthSquared();
    }

public:
    static constexpr Vec2 Zero() {
        return {0.0f, 0.0f};
    }
    static constexpr Vec2 One() {
        return {1.0f, 1.0f};
    }
    static constexpr Vec2 UnitX() {
        return {1.0f, 0.0f};
    }
    static constexpr Vec2 UnitY() {
        return {0.0f, 1.0f};
    }
};

CORE_API constexpr Vec2 operator*(f32 s, Vec2 const& v) {
    return v * s;
}

