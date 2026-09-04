// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <cmath>

#include "Core/CoreTypes.h"

namespace Math {
    template<typename T>
    T Max(T a, T b) noexcept {
        return a > b ? a : b;
    }

    template<typename T>
    T Min(T a, T b) noexcept {
        return a < b ? a : b;
    }

    template<typename T>
    T Clamp(T x, T min, T max) noexcept {
        return Math::Min(Math::Max(x, min), max);
    }

    template<typename T>
    T Abs(T a) noexcept {
        return std::abs(a);
    }

    inline constexpr f32 Sqrt(f32 a) noexcept {
        return std::sqrt(a);
    }

    inline constexpr f64 Sqrt(f64 a) noexcept {
        return std::sqrt(a);
    }

    inline constexpr f32 Cos(f32 a) noexcept {
        return std::cos(a);
    }

    inline constexpr f32 Sin(f32 a) noexcept {
        return std::sin(a);
    }

    inline constexpr f32 Tan(f32 a) noexcept {
        return std::tan(a);
    }

    inline constexpr f32 Acos(f32 a) noexcept {
        return std::acos(a);
    }

    inline constexpr f32 Asin(f32 a) noexcept {
        return std::asin(a);
    }

    inline constexpr f32 Atan(f32 a) noexcept {
        return std::atan(a);
    }

    inline constexpr f32 Atan2(f32 a, f32 b) noexcept {
        return std::atan2(a, b);
    }

    inline constexpr f64 Cos(f64 a) noexcept {
        return std::cos(a);
    }

    inline constexpr f64 Sin(f64 a) noexcept {
        return std::sin(a);
    }

    inline constexpr f64 Tan(f64 a) noexcept {
        return std::tan(a);
    }

    inline constexpr f64 Acos(f64 a) noexcept {
        return std::acos(a);
    }

    inline constexpr f64 Asin(f64 a) noexcept {
        return std::asin(a);
    }

    inline constexpr f64 Atan(f64 a) noexcept {
        return std::atan(a);
    }

    inline constexpr f64 Atan2(f64 a, f64 b) noexcept {
        return std::atan2(a, b);
    }

    static constexpr f32 kEpsilonF32 = std::numeric_limits<f32>::epsilon();
}
