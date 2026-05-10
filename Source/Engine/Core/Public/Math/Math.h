#pragma once
#include <algorithm>

namespace Math {
    template<typename T>
    constexpr T const& Clamp(T const& value, T const& min, T const& max) {
        return std::clamp(value, min, max);
    }
}
