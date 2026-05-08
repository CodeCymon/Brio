#pragma once

#include "CoreAPI.h"

#include <type_traits>

template<typename Type>
CORE_API constexpr std::remove_reference_t<Type>&& MoveTemp(Type&& Object) noexcept {
    using CastType = std::remove_reference_t<Type>;

    static_assert(std::is_lvalue_reference_v<Type>, "MoveTemp called on an rvalue - this is redundant.");
    static_assert(!std::is_same_v<CastType&, CastType const&>, "MoveTemp called on a const object!");

    return (CastType&&)Object;
}

template<typename Type>
CORE_API void Swap(Type& A, Type& B) {
    Type Temp = MoveTemp(A);
    A = MoveTemp(B);
    B = MoveTemp(Temp);
}
