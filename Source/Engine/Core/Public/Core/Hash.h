// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"


constexpr u64 MixBits(u64 x) {
    x *= 0x9E3779B97F4A7C15ULL;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
    x = x ^ (x >> 31);
    return x;
}

template<typename T>
struct Hash;

template<typename T>
struct Hash<T*> {
    u64 operator()(T* ptr) const noexcept {
        return MixBits(reinterpret_cast<u64>(ptr));
    }
};

#define DEFINE_TRIVIAL_HASH(T) \
    template<> \
    struct Hash<T> { \
        u64 operator()(T value) const noexcept { \
            return MixBits(static_cast<u64>(value)); \
        } \
    };

DEFINE_TRIVIAL_HASH(bool)

DEFINE_TRIVIAL_HASH(char)

DEFINE_TRIVIAL_HASH(signed char)

DEFINE_TRIVIAL_HASH(unsigned char)

DEFINE_TRIVIAL_HASH(short)

DEFINE_TRIVIAL_HASH(unsigned short)

DEFINE_TRIVIAL_HASH(int)

DEFINE_TRIVIAL_HASH(unsigned int)

DEFINE_TRIVIAL_HASH(long)

DEFINE_TRIVIAL_HASH(unsigned long)

DEFINE_TRIVIAL_HASH(unsigned long long)