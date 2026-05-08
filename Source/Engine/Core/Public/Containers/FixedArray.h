#pragma once

#include "Log/Assert.h"
#include "Types/CoreTypes.h"

template<typename ElementType, u32 NumElements>
class TFixedArray {
public:
    ElementType ArrayData[NumElements];

    [[nodiscard]] constexpr ElementType& operator[](usize Index) {
        ASSERT(Index < NumElements);
        return ArrayData[Index];
    }

    [[nodiscard]] constexpr ElementType const& operator[](usize Index) const {
         ASSERT(Index < NumElements);
        return ArrayData[Index];
    }

    [[nodiscard]] constexpr bool IsEmpty() const {
        return NumElements == 0;
    }

    [[nodiscard]] constexpr i32 Num() const {
        return NumElements;
    }

    [[nodiscard]] constexpr ElementType* Data() {
        return ArrayData;
    }

    [[nodiscard]] constexpr ElementType const* Data() const {
        return ArrayData;
    }
};

template<typename T, typename... U>
TFixedArray(T, U...) -> TFixedArray<T, 1 + sizeof...(U)>;
