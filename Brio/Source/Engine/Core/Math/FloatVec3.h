#pragma once
#include "Common/Types.h"
#include <cmath>

struct Vec3f
{
    // -- Data --

    union
    {
        struct { f32 x, y, z; };
        f32 data[3];
    };

    // -- Constructors --

    constexpr Vec3f(Vec3f const& v) : x(v.x), y(v.y), z(v.z) {}

    template<typename T>
    explicit constexpr Vec3f(T scalar) : x(scalar), y(scalar), z(scalar) {}

    template<typename T>
    constexpr Vec3f(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

    template<typename X, typename Y, typename Z>
    constexpr Vec3f(X _x, Y _y, Z _z) : x(_x), y(_y), z(_z) {}

    // -- Basic operators --

    f32 const& operator[](const u32 index) const
    {
        return data[index];
    }

    // -- Unary Math operators --

    constexpr Vec3f& operator+=(f32 scalar)
    {
        this->x += scalar;
        this->y += scalar;
        this->z += scalar;
        return *this;
    }

    constexpr Vec3f& operator+=(Vec3f const& vec)
    {
        this->x += vec.x;
        this->y += vec.y;
        this->z += vec.z;
        return *this;
    }

    constexpr Vec3f& operator-=(f32 scalar)
    {
        this->x -= scalar;
        this->y -= scalar;
        this->z -= scalar;
        return *this;
    }

    constexpr Vec3f& operator-=(Vec3f const& vec)
    {
        this->x -= vec.x;
        this->y -= vec.y;
        this->z -= vec.z;
        return *this;
    }

    constexpr Vec3f& operator*=(f32 scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;
        return *this;
    }
    constexpr Vec3f& operator*=(Vec3f const& vec)
    {
        this->x *= vec.x;
        this->y *= vec.y;
        this->z *= vec.z;
        return *this;
    }

    constexpr Vec3f& operator/=(f32 scalar)
    {
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;
        return *this;
    }

    constexpr Vec3f& operator/=(Vec3f const& vec)
    {
        this->x /= vec.x;
        this->y /= vec.y;
        this->z /= vec.z;
        return *this;
    }

    // -- Increment and Decrement --

    constexpr Vec3f& operator++()
    {
        ++this->x;
        ++this->y;
        ++this->z;
        return *this;
    }
    constexpr Vec3f& operator--()
    {
        --this->x;
        --this->y;
        --this->z;
        return *this;
    }

    // -- Property functions --

    f32 length() const
    {
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    constexpr f32 lengthSquared() const
    {
        return (this->x * this->x + this->y * this->y + this->z * this->z);
    }

    bool isZero() const
    {
        return std::abs(this->x) < std::numeric_limits<f32>::epsilon() &&
               std::abs(this->y) < std::numeric_limits<f32>::epsilon() &&
               std::abs(this->z) < std::numeric_limits<f32>::epsilon();
    }
};

// -- Unary operators --

constexpr Vec3f operator+(Vec3f const& v)
{
    return v;
}

constexpr Vec3f operator-(Vec3f const& v)
{
    return Vec3f(0) -= v;
}

// -- Binary operators --

constexpr Vec3f operator+(Vec3f const& v, f32 scalar)
{
    return Vec3f(v) += scalar;
}

constexpr Vec3f operator+(f32 scalar, Vec3f const& v)
{
    return Vec3f(v) += scalar;
}

constexpr Vec3f operator+(Vec3f const& v1, Vec3f const& v2)
{
    return Vec3f(v1) += v2;
}


constexpr Vec3f operator-(Vec3f const& v, f32 scalar)
{
    return Vec3f(v) -= scalar;
}

constexpr Vec3f operator-(f32 scalar, Vec3f const& v)
{
    return Vec3f(v) -= scalar;
}

constexpr Vec3f operator-(Vec3f const& v1, Vec3f const& v2)
{
    return Vec3f(v1) -= v2;
}


constexpr Vec3f operator*(Vec3f const& v, f32 scalar)
{
    return Vec3f(v) *= scalar;
}

constexpr Vec3f operator*(f32 scalar, Vec3f const& v)
{
    return Vec3f(v) *= scalar;
}

constexpr Vec3f operator*(Vec3f const& v1, Vec3f const& v2)
{
    return Vec3f(v1) *= v2;
}


constexpr Vec3f operator/(Vec3f const& v, f32 scalar)
{
    return Vec3f(v) /= scalar;
}

constexpr Vec3f operator/(f32 scalar, Vec3f const& v)
{
    return Vec3f(v) *= scalar;
}

constexpr Vec3f operator/(Vec3f const& v1, Vec3f const& v2)
{
    return Vec3f(v1) /= v2;
}


constexpr bool operator==(Vec3f const& v1, Vec3f const& v2)
{
    return (v1.x == v2.x) &&
           (v1.y == v2.y) &&
           (v1.z == v2.z);
}

// -- Math functions --

constexpr f32 dot(Vec3f const& a, Vec3f const& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr Vec3f cross(Vec3f const& a, Vec3f const& b)
{
    return Vec3f( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}

inline Vec3f normalize(Vec3f const& v)
{
    return Vec3f(v) / v.length();
}

inline Vec3f normalizeSafe(Vec3f const& v, Vec3f const& fallback = Vec3f(1,0,0))
{
    f32 lenSquared = v.lengthSquared();
    if (lenSquared < std::numeric_limits<f32>::epsilon())
        return fallback;
    return v * (1.0f / std::sqrt(lenSquared));
}

constexpr Vec3f lerp(Vec3f const& a, Vec3f const& b, f32 t)
{
    return a + (b - a) * t;
}



#include <format>

template<>
struct std::formatter<Vec3f> {
    std::formatter<float> floatFormatter;

    constexpr auto parse(std::format_parse_context& ctx) {
        return floatFormatter.parse(ctx);  // forward spec to float
    }

    auto format(Vec3f const& v, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");
        out = floatFormatter.format(v.x, ctx);
        out = std::format_to(out, ", ");
        out = floatFormatter.format(v.y, ctx);
        out = std::format_to(out, ", ");
        out = floatFormatter.format(v.z, ctx);
        return std::format_to(out, ")");
    }
};
