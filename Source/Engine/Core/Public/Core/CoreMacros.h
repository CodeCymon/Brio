// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <type_traits>

#define NON_COPYABLE(ClassName) \
    ClassName(ClassName const&) = delete; \
    ClassName& operator=(ClassName const&) = delete;

#define NON_MOVEABLE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define ENABLE_ENUM_BITWISE_OPERATORS(Enum)                              \
    inline constexpr Enum operator|(Enum lhs, Enum rhs) {                \
        using U = std::underlying_type_t<Enum>;                          \
        return static_cast<Enum>(static_cast<U>(lhs) | static_cast<U>(rhs)); \
    }                                                                    \
    inline constexpr Enum operator&(Enum lhs, Enum rhs) {                \
        using U = std::underlying_type_t<Enum>;                          \
        return static_cast<Enum>(static_cast<U>(lhs) & static_cast<U>(rhs)); \
    }                                                                    \
    inline constexpr Enum operator^(Enum lhs, Enum rhs) {                \
        using U = std::underlying_type_t<Enum>;                          \
        return static_cast<Enum>(static_cast<U>(lhs) ^ static_cast<U>(rhs)); \
    }                                                                    \
    inline constexpr Enum operator~(Enum value) {                        \
        using U = std::underlying_type_t<Enum>;                          \
        return static_cast<Enum>(~static_cast<U>(value));                \
    }                                                                    \
    inline constexpr Enum& operator|=(Enum& lhs, Enum rhs) {             \
        return lhs = lhs | rhs;                                          \
    }                                                                    \
    inline constexpr Enum& operator&=(Enum& lhs, Enum rhs) {             \
        return lhs = lhs & rhs;                                          \
    }                                                                    \
    inline constexpr Enum& operator^=(Enum& lhs, Enum rhs) {             \
        return lhs = lhs ^ rhs;                                          \
    }

