// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Math/Vector3.h"

// 4-Component floating point vector
struct CORE_API Vec4 {
    f32 x, y, z, w;

public:
    constexpr Vec4() : x(0), y(0), z(0), w(0) {}

    constexpr explicit Vec4(f32 s) : x(s), y(s), z(s), w(s) {}

    constexpr Vec4(f32 x_, f32 y_, f32 z_, f32 w_) : x(x_), y(y_), z(z_), w(w_) {}

    constexpr explicit Vec4(Vec2 const& v) : x(v.x), y(v.y), z(0), w(0) {}

    constexpr Vec4(Vec2 const& v, f32 z_, f32 w_) : x(v.x), y(v.y), z(z_), w(w_) {}

    constexpr explicit Vec4(Vec3 const& v) : x(v.x), y(v.y), z(v.z), w(0) {}

    constexpr Vec4(Vec3 const& v, f32 w_) : x(v.x), y(v.y), z(v.z), w(w_) {}

public:
    constexpr f32& operator[](i32 i) {
        ASSERT(i >= 0 && i < 4);
        return *(&x + i);
    }

    constexpr f32 operator[](i32 i) const {
        ASSERT(i >= 0 && i < 4);
        return *(&x + i);
    }

    constexpr Vec4 operator+(Vec4 const& v) const {
        return {x + v.x, y + v.y, z + v.z, w + v.w};
    }

    constexpr Vec4 operator-(Vec4 const& v) const {
        return {x - v.x, y - v.y, z - v.z, w - v.w};
    }

    constexpr Vec4 operator*(Vec4 const& v) const {
        return {x * v.x, y * v.y, z * v.z, w * v.w};
    }

    constexpr Vec4 operator*(f32 s) const {
        return {x * s, y * s, z * s, w * s};
    }

    constexpr Vec4 operator/(f32 s) const {
        f32 const invS = 1.0f / s;
        return {x * invS, y * invS, z * invS, w * invS};
    }

    constexpr Vec4 operator-() const {
        return {-x, -y, -z, -w};
    }


    Vec4& operator+=(Vec4 const& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    Vec4& operator-=(Vec4 const& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Vec4& operator*=(Vec4 const& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    Vec4& operator*=(f32 s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    Vec4& operator/=(f32 s) {
        f32 const invS = 1.0f / s;
        x *= invS;
        y *= invS;
        z *= invS;
        w *= invS;
        return *this;
    }

public:
    constexpr bool operator==(Vec4 const& v) const {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    constexpr bool operator!=(Vec4 const& v) const {
        return !(*this == v);
    }

public:
    [[nodiscard]] constexpr f32 LengthSquared() const {  return x * x + y * y + z * z + w * w; }
    [[nodiscard]] constexpr f32 Length() const {  return Math::Sqrt(LengthSquared()); }

    [[nodiscard]] Vec4 Normalized() const {
        f32 const lenSquared = LengthSquared();
        ASSERT(lenSquared > Math::kEpsilonF32 * Math::kEpsilonF32);
        f32 const invLen = 1.0f / Math::Sqrt(lenSquared);
        return {x * invLen, y * invLen, z * invLen, w * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

public:
    static constexpr f32 Dot(Vec4 const& a, Vec4 const& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

     static constexpr f32 Distance(Vec4 const& a, Vec4 const& b) {
        return (b - a).Length();
    }

    static constexpr f32 DistanceSquared(Vec4 const& a, Vec4 const& b) {
        return (b - a).LengthSquared();
    }

public:
    static constexpr Vec4 Zero() {
        return {0,0,0,0};
    }
    static constexpr Vec4 One() {
        return {1,1,1,1};
    }
    static constexpr Vec4 UnitX() {
        return {1,0,0,0};
    }
    static constexpr Vec4 UnitY() {
        return {0,1,0,0};
    }
    static constexpr Vec4 UnitZ() {
        return {0,0,1,0};
    }
    static constexpr Vec4 UnitW() {
        return {0,0,0,1};
    }
};

CORE_API constexpr Vec4 operator*(f32 s, Vec4 const& v) {
    return v * s;
}


