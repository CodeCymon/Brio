// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"
#include "Log/Assert.h"
#include "Math/Vector2.h"

struct Vector3 {
    f32 x, y, z;

    static constexpr f32 kEpsilon = 1e-8f;

    // Constructors

    constexpr Vector3() : x(0), y(0), z(0) {}

    constexpr explicit Vector3(f32 s) : x(s), y(s), z(s) {}

    constexpr Vector3(f32 x_, f32 y_, f32 z_) : x(x_), y(y_), z(z_) {}

    constexpr explicit Vector3(Vector2 const& v) : x(v.x), y(v.y), z(0) {}

    constexpr Vector3(Vector2 const& v, f32 z_) : x(v.x), y(v.y), z(z_) {}

    // Operators

    constexpr Vector3 operator+(Vector3 const& v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    constexpr Vector3 operator-(Vector3 const& v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    constexpr Vector3 operator*(Vector3 const& v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    constexpr Vector3 operator*(f32 s) const {
        return {x * s, y * s, z * s};
    }

    constexpr Vector3 operator/(f32 s) const {
        f32 const invS = 1.0f / s;
        return {x * invS, y * invS, z * invS};
    }

    constexpr Vector3 operator-() const {
        return {-x, -y, -z};
    }


    Vector3& operator+=(Vector3 const& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3& operator-=(Vector3 const& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3& operator*=(Vector3 const& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    Vector3& operator*=(f32 s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3& operator/=(f32 s) {
        f32 const invS = 1.0f / s;
        x *= invS;
        y *= invS;
        z *= invS;
        return *this;
    }

    // Comparators

    constexpr bool operator==(Vector3 const& v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    constexpr bool operator!=(Vector3 const& v) const {
        return !(*this == v);
    }

    // Functions

    [[nodiscard]] constexpr f32 LengthSquared() const {  return x * x + y * y + z * z; }
    [[nodiscard]] constexpr f32 Length() const {  return std::sqrt(LengthSquared()); }

    [[nodiscard]] Vector3 Normalized() const {
        f32 const lenSquared = LengthSquared();
        ASSERT(lenSquared > kEpsilon * kEpsilon);
        f32 const invLen = 1.0f / std::sqrt(lenSquared);
        return {x * invLen, y * invLen, z * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

    // Static Functions

    static constexpr f32 Dot(Vector3 const& a, Vector3 const& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static constexpr Vector3 Cross(Vector3 const& a, Vector3 const& b) {
        return { a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x };
    }

     static constexpr f32 Distance(Vector3 const& a, Vector3 const& b) {
        return (b - a).Length();
    }

    static constexpr f32 DistanceSquared(Vector3 const& a, Vector3 const& b) {
        return (b - a).LengthSquared();
    }

    // Default Vectors

    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 Forward;
    static const Vector3 Right;
    static const Vector3 Up;
};

constexpr Vector3 operator*(f32 s, Vector3 const& v) {
    return v * s;
}


