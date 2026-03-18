#pragma once
#include <cmath>

#include "Common/Types.h"

struct Vec2i
{
    // -- Data --
    
    union
    {
        struct { i32 x; i32 y; };
        i32 data[2];
    };

    // -- Constructors --

    constexpr Vec2i(Vec2i const& v) : x(v.x), y(v.y) {}

    template<typename T>
    explicit constexpr Vec2i(T scalar) : x(scalar), y(scalar) {}

    template<typename T>
    constexpr Vec2i(T _x, T _y) : x(_x), y(_y) {}

    template<typename X, typename Y>
    constexpr Vec2i(X _x, Y _y) : x(_x), y(_y) {}

    // -- Basic operators --

    i32 const& operator[](const u32 index) const
    {
        return data[index];
    }

        // -- Unary Math operators --

    constexpr Vec2i& operator+=(i32 scalar)
    {
        this->x += scalar;
        this->y += scalar;
        return *this;
    }

    constexpr Vec2i& operator+=(Vec2i const& vec)
    {
        this->x += vec.x;
        this->y += vec.y;
        return *this;
    }

    constexpr Vec2i& operator-=(i32 scalar)
    {
        this->x -= scalar;
        this->y -= scalar;
        return *this;
    }

    constexpr Vec2i& operator-=(Vec2i const& vec)
    {
        this->x -= vec.x;
        this->y -= vec.y;
        return *this;
    }

    constexpr Vec2i& operator*=(i32 scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
        return *this;
    }
    constexpr Vec2i& operator*=(Vec2i const& vec)
    {
        this->x *= vec.x;
        this->y *= vec.y;
        return *this;
    }

    constexpr Vec2i& operator/=(i32 scalar)
    {
        this->x /= scalar;
        this->y /= scalar;
        return *this;
    }

    constexpr Vec2i& operator/=(Vec2i const& vec)
    {
        this->x /= vec.x;
        this->y /= vec.y;
        return *this;
    }

    // -- Increment and Decrement --

    constexpr Vec2i& operator++()
    {
        ++this->x;
        ++this->y;
        return *this;
    }
    constexpr Vec2i& operator--()
    {
        --this->x;
        --this->y;
        return *this;
    }

    // -- Property functions --

    f32 length() const
    {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    constexpr i32 lengthSquared() const
    {
        return (this->x * this->x + this->y * this->y);
    }

    bool isZero() const
    {
        return this->x == 0 && this->y == 0;
    }
};

// -- Unary operators --

constexpr Vec2i operator+(Vec2i const& v)
{
    return v;
}

constexpr Vec2i operator-(Vec2i const& v)
{
    return Vec2i(0) -= v;
}

// -- Binary operators --

constexpr Vec2i operator+(Vec2i const& v, i32 scalar)
{
    return Vec2i(v) += scalar;
}

constexpr Vec2i operator+(i32 scalar, Vec2i const& v)
{
    return Vec2i(v) += scalar;
}

constexpr Vec2i operator+(Vec2i const& v1, Vec2i const& v2)
{
    return Vec2i(v1) += v2;
}


constexpr Vec2i operator-(Vec2i const& v, i32 scalar)
{
    return Vec2i(v) -= scalar;
}

constexpr Vec2i operator-(i32 scalar, Vec2i const& v)
{
    return Vec2i(v) -= scalar;
}

constexpr Vec2i operator-(Vec2i const& v1, Vec2i const& v2)
{
    return Vec2i(v1) -= v2;
}


constexpr Vec2i operator*(Vec2i const& v, i32 scalar)
{
    return Vec2i(v) *= scalar;
}

constexpr Vec2i operator*(i32 scalar, Vec2i const& v)
{
    return Vec2i(v) *= scalar;
}

constexpr Vec2i operator*(Vec2i const& v1, Vec2i const& v2)
{
    return Vec2i(v1) *= v2;
}


constexpr Vec2i operator/(Vec2i const& v, i32 scalar)
{
    return Vec2i(v) /= scalar;
}

constexpr Vec2i operator/(i32 scalar, Vec2i const& v)
{
    return Vec2i(v) *= scalar;
}

constexpr Vec2i operator/(Vec2i const& v1, Vec2i const& v2)
{
    return Vec2i(v1) /= v2;
}


constexpr bool operator==(Vec2i const& v1, Vec2i const& v2)
{
    return (v1.x == v2.x) &&
           (v1.y == v2.y);
}

// -- Math functions --

constexpr f32 dot(Vec2i const& a, Vec2i const& b)
{
    return a.x * b.x + a.y * b.y;
}

constexpr f32 cross(Vec2i const& a, Vec2i const& b)
{
    return a.x * b.y - a.y * b.x;
}

inline Vec2i normalize(Vec2i const& v)
{
    return Vec2i(v) / v.length();
}

inline Vec2i normalizeSafe(Vec2i const& v, Vec2i const& fallback = Vec2i(1,0))
{
    i32 lenSquared = v.lengthSquared();
    if (lenSquared == 0)
        return fallback;
    return v * (1.0f / std::sqrt(static_cast<f32>(lenSquared)));
}

constexpr Vec2i lerp(Vec2i const& a, Vec2i const& b, f32 t)
{
    return a + (b - a) * t;
}



#include <format>

template<>
struct std::formatter<Vec2i> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(Vec2i const& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};