// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Math/Vector.h"

struct Vector4 {
    f32 x, y, z, w;

    static constexpr f32 kEpsilon = 1e-8f;

    // Constructors

    constexpr Vector4() : x(0), y(0), z(0), w(0) {}

    constexpr explicit Vector4(f32 s) : x(s), y(s), z(s), w(s) {}

    constexpr Vector4(f32 x_, f32 y_, f32 z_, f32 w_) : x(x_), y(y_), z(z_), w(w_) {}

    constexpr explicit Vector4(Vector2 const& v) : x(v.x), y(v.y), z(0), w(0) {}

    constexpr Vector4(Vector2 const& v, f32 z_, f32 w_) : x(v.x), y(v.y), z(z_), w(w_) {}

    constexpr explicit Vector4(Vector3 const& v) : x(v.x), y(v.y), z(v.z), w(0) {}

    constexpr Vector4(Vector3 const& v, f32 w_) : x(v.x), y(v.y), z(v.z), w(w_) {}

    // Operators

    constexpr Vector4 operator+(Vector4 const& v) const {
        return {x + v.x, y + v.y, z + v.z, w + v.w};
    }

    constexpr Vector4 operator-(Vector4 const& v) const {
        return {x - v.x, y - v.y, z - v.z, w - v.w};
    }

    constexpr Vector4 operator*(Vector4 const& v) const {
        return {x * v.x, y * v.y, z * v.z, w * v.w};
    }

    constexpr Vector4 operator*(f32 s) const {
        return {x * s, y * s, z * s, w * s};
    }

    constexpr Vector4 operator/(f32 s) const {
        f32 const invS = 1.0f / s;
        return {x * invS, y * invS, z * invS, w * invS};
    }

    constexpr Vector4 operator-() const {
        return {-x, -y, -z, -w};
    }


    Vector4& operator+=(Vector4 const& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    Vector4& operator-=(Vector4 const& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Vector4& operator*=(Vector4 const& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    Vector4& operator*=(f32 s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    Vector4& operator/=(f32 s) {
        f32 const invS = 1.0f / s;
        x *= invS;
        y *= invS;
        z *= invS;
        w *= invS;
        return *this;
    }

    // Comparators

    constexpr bool operator==(Vector4 const& v) const {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    constexpr bool operator!=(Vector4 const& v) const {
        return !(*this == v);
    }

    // Functions

    [[nodiscard]] constexpr f32 LengthSquared() const {  return x * x + y * y + z * z + w * w; }
    [[nodiscard]] constexpr f32 Length() const {  return std::sqrt(LengthSquared()); }

    [[nodiscard]] Vector4 Normalized() const {
        f32 const lenSquared = LengthSquared();
        ASSERT(lenSquared > kEpsilon * kEpsilon);
        f32 const invLen = 1.0f / std::sqrt(lenSquared);
        return {x * invLen, y * invLen, z * invLen, w * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

    // Static Functions

    static constexpr f32 Dot(Vector4 const& a, Vector4 const& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

     static constexpr f32 Distance(Vector4 const& a, Vector4 const& b) {
        return (b - a).Length();
    }

    static constexpr f32 DistanceSquared(Vector4 const& a, Vector4 const& b) {
        return (b - a).LengthSquared();
    }

    // Default Vectors

    static const Vector4 Zero;
    static const Vector4 One;
    static const Vector4 UnitX;
    static const Vector4 UnitY;
    static const Vector4 UnitZ;
    static const Vector4 UnitW;
};

constexpr Vector4 operator*(f32 s, Vector4 const& v) {
    return v * s;
}


