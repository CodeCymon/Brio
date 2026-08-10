// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"
#include "Log/Assert.h"

struct CORE_API Vector2 {
    f32 x, y;

    static constexpr f32 kEpsilon = 1e-8f;

    // Constructors

    constexpr Vector2() : x(0), y(0) {}

    constexpr explicit Vector2(f32 s) : x(s), y(s) {}

    constexpr Vector2(f32 x_, f32 y_) : x(x_), y(y_) {}

    // Operators

    constexpr Vector2 operator+(Vector2 const& v) const {
        return {x + v.x, y + v.y};
    }

    constexpr Vector2 operator-(Vector2 const& v) const {
        return {x - v.x, y - v.y};
    }

    constexpr Vector2 operator*(Vector2 const& v) const {
        return {x * v.x, y * v.y};
    }

    constexpr Vector2 operator*(f32 s) const {
        return {x * s, y * s};
    }

    constexpr Vector2 operator/(f32 s) const {
        f32 const invS = 1.0f / s;
        return {x * invS, y * invS};
    }

    constexpr Vector2 operator-() const {
        return {-x, -y};
    }


    Vector2& operator+=(Vector2 const& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator-=(Vector2 const& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& operator*=(Vector2 const& v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    Vector2& operator*=(f32 s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2& operator/=(f32 s) {
        f32 const invS = 1.0f / s;
        x *= invS;
        y *= invS;
        return *this;
    }

    // Comparators

    constexpr bool operator==(Vector2 const& v) const {
        return x == v.x && y == v.y;
    }

    constexpr bool operator!=(Vector2 const& v) const {
        return !(*this == v);
    }

    // Functions

    [[nodiscard]] constexpr f32 LengthSquared() const {  return x * x + y * y; }
    [[nodiscard]] constexpr f32 Length() const {  return std::sqrt(LengthSquared()); }

    [[nodiscard]] Vector2 Normalized() const {
        f32 const lenSquared = LengthSquared();
        ASSERT(lenSquared > kEpsilon * kEpsilon);
        f32 const invLen = 1.0f / std::sqrt(lenSquared);
        return {x * invLen, y * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

    // Static Functions

    static constexpr f32 Dot(Vector2 const& a, Vector2 const& b) {
        return a.x * b.x + a.y * b.y;
    }

    static constexpr f32 Distance(Vector2 const& a, Vector2 const& b) {
        return (b - a).Length();
    }

    static constexpr f32 DistanceSquared(Vector2 const& a, Vector2 const& b) {
        return (b - a).LengthSquared();
    }

    // Default Vectors

    static const Vector2 Zero;
    static const Vector2 One;
    static const Vector2 UnitX;
    static const Vector2 UnitY;
};

constexpr Vector2 operator*(f32 s, Vector2 const& v) {
    return v * s;
}

