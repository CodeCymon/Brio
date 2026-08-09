// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <type_traits>

/**
 * Cast object to an rvalue.
 * Same as "MoveIfPossible(...)" but will static_assert when it won't work.
 */
template<typename T>
constexpr std::remove_reference_t<T>&& Move(T&& object) noexcept {
    using CastType = std::remove_reference_t<T>;
    static_assert(std::is_lvalue_reference_v<T>, "Move called on: rvalue");
    static_assert(!std::is_same_v<CastType&, const CastType&>, "Move called on: const object");
    return static_cast<CastType&&>(object);
}

/**
 * Cast object to an rvalue.
 */
template<typename T>
constexpr std::remove_reference_t<T>&& MoveIfPossible(T&& object) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(object);
}