// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "CoreAPI.h"
#include "Core/CoreTypes.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

struct CORE_API Quat {
    f32 x, y, z, w;

public:
    constexpr Quat() : x(0), y(0), z(0), w(1) {}

    constexpr Quat(f32 x_, f32 y_, f32 z_, f32 w_)
        : x(x_), y(y_), z(z_), w(w_) {}

    constexpr Quat(Vec3 const& axis, f32 radians) {
        Vec3 const a = axis.Normalized();
        f32 const h = radians * 0.5f;
        f32 const s = Math::Sin(h);

        x = a.x * s;
        y = a.y * s;
        z = a.z * s;
        w = Math::Cos(h);
    }

    constexpr Quat(f32 pitch, f32 yaw, f32 roll) {
        f32 const cp = Math::Cos(pitch * 0.5f);
        f32 const sp = Math::Sin(pitch * 0.5f);
        f32 const cy = Math::Cos(yaw * 0.5f);
        f32 const sy = Math::Sin(yaw * 0.5f);
        f32 const cr = Math::Cos(roll * 0.5f);
        f32 const sr = Math::Sin(roll * 0.5f);

        x = sp * cy * cr - cp * sy * sr;
        y = cp * sy * cr + sp * cy * sr;
        z = cp * cy * sr - sp * sy * cr;
        w = cp * cy * cr + sp * sy * sr;
    }

    /**
     * Assumes that the input matrix is a pure rotation matrix.
     */
    constexpr Quat(Mat4 const& m) {
        f32 const trace = m.At(0,0) + m.At(1,1) + m.At(2,2);
        if (trace > 0.0f) {
            f32 const s = Math::Sqrt(trace + 1.0f) * 2.0f;
            w = 0.25f * s;
            x = (m.At(2, 1) - m.At(1, 2)) / s;
            y = (m.At(0, 2) - m.At(2, 0)) / s;
            z = (m.At(1, 0) - m.At(0, 1)) / s;
        } else if (m.At(0, 0) > m.At(1, 1) && m.At(0, 0) > m.At(2, 2)) {
            f32 const s = Math::Sqrt(1.0f + m.At(0, 0) - m.At(1, 1) - m.At(2, 2)) * 2.0f; // s = 4 * qx
            w = (m.At(2, 1) - m.At(1, 2)) / s;
            x = 0.25f * s;
            y = (m.At(0, 1) + m.At(1, 0)) / s;
            z = (m.At(0, 2) + m.At(2, 0)) / s;
        } else if (m.At(1, 1) > m.At(2, 2)) {
            f32 const s = Math::Sqrt(1.0f + m.At(1, 1) - m.At(0, 0) - m.At(2, 2)) * 2.0f; // s = 4 * qy
            w = (m.At(0, 2) - m.At(2, 0)) / s;
            x = (m.At(0, 1) + m.At(1, 0)) / s;
            y = 0.25f * s;
            z = (m.At(1, 2) + m.At(2, 1)) / s;
        } else {
            f32 const s = Math::Sqrt(1.0f + m.At(2, 2) - m.At(0, 0) - m.At(1, 1)) * 2.0f; // s = 4 * qz
            w = (m.At(1, 0) - m.At(0, 1)) / s;
            x = (m.At(0, 2) + m.At(2, 0)) / s;
            y = (m.At(1, 2) + m.At(2, 1)) / s;
            z = 0.25f * s;
        }
    }

public:
    Quat operator*(Quat const& o) const {
        return {
            w * o.x + x * o.w + y * o.z - z * o.y,
            w * o.y - x * o.z + y * o.w + z * o.x,
            w * o.z + x * o.y - y * o.x + z * o.w,
            w * o.w - x * o.x - y * o.y + z * o.z
        };
    }

    Quat operator+(Quat const& o) const {
        return {x + o.x, y + o.y, z + o.z, w + o.w};
    }

    Quat operator-(Quat const& o) const {
        return {x - o.x, y - o.y, z - o.z, w - o.w};
    }

    Quat operator*(f32 scalar) const {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    Quat operator-() const {
        return {-x, -y, -z, -w};
    }

public:
    static constexpr f32 Dot(Quat const& a, Quat const& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

public:
    [[nodiscard]] f32 LengthSquared() const { return x * x + y * y + z * z + w * w; }
    [[nodiscard]] f32 Length() const { return Math::Sqrt(LengthSquared()); }

    [[nodiscard]] Quat Normalized() const {
        f32 const len = Length();
        ASSERT(len > Math::kEpsilonF32);
        f32 const invLen = 1.0f / len;
        return {x * invLen, y * invLen, z * invLen, w * invLen};
    }

    void Normalize() {
        *this = Normalized();
    }

    [[nodiscard]] Quat Conjugate() const {
        return {x, -y, -z, w};
    }

    [[nodiscard]] Quat Inverse() const {
        f32 const lenSq = LengthSquared();
        ASSERT(lenSq > Math::kEpsilonF32);
        f32 const invLenSq = 1.0f / lenSq;
        return Conjugate() * invLenSq;
    }

    [[nodiscard]] Vec3 RotateVector(Vec3 const& v) const {
        Vec3 const qv{x,y,z};
        Vec3 const t = Vec3::Cross(qv, v) * 2.0f;
        return v + t * w + Vec3::Cross(qv, t);
    }

    [[nodiscard]] Mat4 ToMatrix() const {
        f32 const xx = x * x;
        f32 const yy = y * y;
        f32 const zz = z * z;
        f32 const xy = x * y;
        f32 const xz = x * z;
        f32 const yz = y * z;
        f32 const wx = w * x;
        f32 const wy = w * y;
        f32 const wz = w * z;

        return {
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),        2.0f * (xz + wy),        0.0f,
            2.0f * (xy + wz),        1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),        0.0f,
            2.0f * (xz - wy),        2.0f * (yz + wx),        1.0f - 2.0f * (xx + yy), 0.0f,
            0.0f,                    0.0f,                    0.0f,                    1.0f
        };
    }

public:
        /**
         * Nlerp - cheap approximate interpolation, good for blending small deltas per-frame.
         */
    static Quat Lerp(Quat const& a, Quat const& b, f32 t) {
        f32 const dot = a.Dot(b);
        Quat const bAdj = dot < 0.0f ? -b : b;
        return (a * (1.0f - t) + bAdj * t).Normalized();
    }

    /**
     * Slerp - accurate interpolation, when constant angular velocity actually matters.
     */
    static Quat Slerp(Quat const& a, Quat const& b, f32 t) {
        f32 dot = a.Dot(b);
        Quat bAdj = b;
        if (dot < 0.0f) { bAdj = -b; dot = -dot; }

        constexpr f32 kEpsilonDot = 0.9995f;
        if (dot > kEpsilonDot) {
            return Lerp(a, bAdj, t);
        }

        f32 const theta0 = Math::Acos(dot);
        f32 const theta = theta0 * t;
        f32 const sinTheta0 = Math::Sin(theta0);
        f32 const s0 = Math::Sin(theta0 - theta) / sinTheta0;
        f32 const s1 = Math::Sin(theta) / sinTheta0;

        return (a * s0) + (bAdj * s1);
    }

public:
    static constexpr Quat Identity() {
        return Quat{0,0,0,1};
    }
};
