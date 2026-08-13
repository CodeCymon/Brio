// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Math/Vector4.h"
#include "Math/Math.h"

struct CORE_API Mat4 {
    f32 m[4][4];

public:
    constexpr Mat4() : m{} {}

    constexpr explicit Mat4(f32 diagonal) : m{
        diagonal, 0, 0, 0,
        0, diagonal, 0, 0,
        0, 0, diagonal, 0,
        0, 0, 0, diagonal
     } {}

    constexpr Mat4(Vec4 const& row0, Vec4 const& row1, Vec4 const& row2, Vec4 const& row3) : m{
        row0[0], row0[1], row0[2], row0[3],
        row1[0], row1[1], row1[2], row1[3],
        row2[0], row2[1], row2[2], row2[3],
        row3[0], row3[1], row3[2], row3[3]
    } {}

    constexpr Mat4(f32 m00, f32 m01, f32 m02, f32 m03,
                   f32 m10, f32 m11, f32 m12, f32 m13,
                   f32 m20, f32 m21, f32 m22, f32 m23,
                   f32 m30, f32 m31, f32 m32, f32 m33) : m{
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    } {}

public:
    constexpr f32& At(i32 row, i32 column) {
        return m[row][column];
    }

    [[nodiscard]] constexpr f32 At(i32 row, i32 column) const {
        return m[row][column];
    }

    Mat4 operator*(Mat4 const& o) const {
        Mat4 result{};
        for (i32 row = 0; row < 4; ++row) {
            for (i32 col = 0; col < 4; ++col) {
                for (i32 k = 0; k < 4; ++k)
                    result.m[row][col] += m[row][k] * o.m[k][col];
            }
        }
        return result;
    }

    Vec4 operator*(Vec4 const& v) const {
        return {
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        };
    }

public:
    [[nodiscard]] Vec3 TransformPoint(Vec3 const& p) const {
        Vec4 r = *this * Vec4(p, 1.0f);
        return {r.x, r.y, r.z};
    }

    [[nodiscard]] Vec3 TransformDirection(Vec3 const& d) const {
        Vec4 r = *this * Vec4(d, 0.0f);
        return {r.x, r.y, r.z};
    }

    [[nodiscard]] Mat4 Transpose() const {
        Mat4 t{};
        for (i32 row = 0; row < 4; ++row) {
            for (i32 col = 0; col < 4; ++col) {
                t.m[col][row] = m[row][col];
            }
        }
        return t;
    }

    [[nodiscard]] Mat4 Inverse() const {
        f32 const A2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
        f32 const A1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
        f32 const A1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
        f32 const A0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
        f32 const A0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
        f32 const A0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
        f32 const A2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
        f32 const A1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
        f32 const A1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
        f32 const A2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
        f32 const A1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
        f32 const A1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        f32 const A0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
        f32 const A0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
        f32 const A0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
        f32 const A0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
        f32 const A0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
        f32 const A0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

        f32 const det =
          m[0][0] * (m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223)
        - m[0][1] * (m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223)
        + m[0][2] * (m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123)
        - m[0][3] * (m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123);

        ASSERT(Math::Abs(det) > Math::kEpsilonF32);
        f32 const invDet = 1.0f / det;

        Mat4 result{};

        result.m[0][0] =  invDet * (m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223);
        result.m[0][1] = -invDet * (m[0][1] * A2323 - m[0][2] * A1323 + m[0][3] * A1223);
        result.m[0][2] =  invDet * (m[0][1] * A2313 - m[0][2] * A1313 + m[0][3] * A1213);
        result.m[0][3] = -invDet * (m[0][1] * A2312 - m[0][2] * A1312 + m[0][3] * A1212);

        result.m[1][0] = -invDet * (m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223);
        result.m[1][1] =  invDet * (m[0][0] * A2323 - m[0][2] * A0323 + m[0][3] * A0223);
        result.m[1][2] = -invDet * (m[0][0] * A2313 - m[0][2] * A0313 + m[0][3] * A0213);
        result.m[1][3] =  invDet * (m[0][0] * A2312 - m[0][2] * A0312 + m[0][3] * A0212);

        result.m[2][0] =  invDet * (m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123);
        result.m[2][1] = -invDet * (m[0][0] * A1323 - m[0][1] * A0323 + m[0][3] * A0123);
        result.m[2][2] =  invDet * (m[0][0] * A1313 - m[0][1] * A0313 + m[0][3] * A0113);
        result.m[2][3] = -invDet * (m[0][0] * A1312 - m[0][1] * A0312 + m[0][3] * A0112);

        result.m[3][0] = -invDet * (m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123);
        result.m[3][1] =  invDet * (m[0][0] * A1223 - m[0][1] * A0223 + m[0][2] * A0123);
        result.m[3][2] = -invDet * (m[0][0] * A1213 - m[0][1] * A0213 + m[0][2] * A0113);
        result.m[3][3] =  invDet * (m[0][0] * A1212 - m[0][1] * A0212 + m[0][2] * A0112);

        return result;
    }

public:
    static Mat4 Translation(Vec3 const& t) {
        return {
            1, 0, 0, t.x,
            0, 1, 0, t.y,
            0, 0, 1, t.z,
            0, 0, 0, 1
        };
    }

    static Mat4 Scale(Vec3 const& s) {
        return {
            s.x, 0,   0,   0,
            0,   s.y, 0,   0,
            0,   0,   s.z, 0,
            0,   0,   0,   1
        };
    }

    static Mat4 RotationAxis(Vec3 const& axis, f32 radians) {
        Vec3 const a = axis.Normalized();

        f32 const c = Math::Cos(radians);
        f32 const s = Math::Sin(radians);
        f32 const t = 1.0f - c;

        return {
            t*a.x*a.x + c,      t*a.x*a.y - s*a.z,  t*a.x*a.z + s*a.y, 0.0f,
            t*a.x*a.y + s*a.z,  t*a.y*a.y + c,      t*a.y*a.z - s*a.x, 0.0f,
            t*a.x*a.z - s*a.y,  t*a.y*a.z + s*a.x,  t*a.z*a.z + c,     0.0f,
            0.0f,               0.0f,               0.0f,              1.0f
        };
    }

public:
    static constexpr Mat4 Identity() {
        return Mat4{1.0f};
    }
};