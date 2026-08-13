// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Math/Vector2.h"

// 3-Component floating point vector
struct CORE_API Vec3 {
    f32 x, y, z;

public:
    constexpr Vec3() : x(0), y(0), z(0) {}

    constexpr explicit Vec3(f32 s) : x(s), y(s), z(s) {}

    constexpr Vec3(f32 x_, f32 y_, f32 z_) : x(x_), y(y_), z(z_) {}

    constexpr explicit Vec3(Vec2 const& v) : x(v.x), y(v.y), z(0) {}

    constexpr Vec3(Vec2 const& v, f32 z_) : x(v.x), y(v.y), z(z_) {}

public:
    constexpr f32& operator[](i32 i) {
        ASSERT(i >= 0 && i < 3);
        return *(&x + i);
    }

    constexpr f32 operator[](i32 i) const {
        ASSERT(i >= 0 && i < 3);
        return *(&x + i);
    }

    constexpr Vec3 operator+(Vec3 const& v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    constexpr Vec3 operator-(Vec3 const& v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    constexpr Vec3 operator*(Vec3 const& v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    constexpr Vec3 operator*(f32 s) const {
        return {x * s, y * s, z * s};
    }

    constexpr Vec3 operator/(f32 s) const {
        f32 const invS = 1.0f / s;
        return {x * invS, y * invS, z * invS};
    }

    constexpr Vec3 operator-() const {
        return {-x, -y, -z};
    }


    Vec3& operator+=(Vec3 const& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(Vec3 const& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(Vec3 const& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    Vec3& operator*=(f32 s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vec3& operator/=(f32 s) {
        f32 const invS = 1.0f / s;
        x *= invS;
        y *= invS;
        z *= invS;
        return *this;
    }

public:
    constexpr bool operator==(Vec3 const& v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    constexpr bool operator!=(Vec3 const& v) const {
        return !(*this == v);
    }

public:
    [[nodiscard]] constexpr f32 LengthSquared() const {  return x * x + y * y + z * z; }
    [[nodiscard]] constexpr f32 Length() const {  return Math::Sqrt(LengthSquared()); }

    [[nodiscard]] Vec3 Normalized() const {
        f32 const lenSquared = LengthSquared();
        ASSERT(lenSquared > Math::kEpsilonF32 * Math::kEpsilonF32);
        f32 const invLen = 1.0f / Math::Sqrt(lenSquared);
        return {x * invLen, y * invLen, z * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

public:
    static constexpr f32 Dot(Vec3 const& a, Vec3 const& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static constexpr Vec3 Cross(Vec3 const& a, Vec3 const& b) {
        return { a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x };
    }

     static constexpr f32 Distance(Vec3 const& a, Vec3 const& b) {
        return (b - a).Length();
    }

    static constexpr f32 DistanceSquared(Vec3 const& a, Vec3 const& b) {
        return (b - a).LengthSquared();
    }

public:
    static constexpr Vec3 Zero() {
        return {0,0,0};
    }
    static constexpr Vec3 One() {
        return {1,1,1};
    }
    static constexpr Vec3 Forward() {
        return {0,1,0};
    }
    static constexpr Vec3 Right() {
        return {1,0,0};
    }
    static constexpr Vec3 Up() {
        return {0,0,1};
    }
};

CORE_API constexpr Vec3 operator*(f32 s, Vec3 const& v) {
    return v * s;
}


