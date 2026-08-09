// Copyright (c) Simon Kirsch 2026.

#pragma once

namespace Math {
    template<typename T>
    T Max(T a, T b) noexcept {
        return a > b ? a : b;
    }

    template<typename T>
    T Min(T a, T b) noexcept {
        return a < b ? a : b;
    }
}