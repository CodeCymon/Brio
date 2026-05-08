#pragma once

#include "Types/CoreTypes.h"

template<typename Type>
struct THandle {
    u32 index       : 20 {0};
    u32 generation  : 12 {0};

    [[nodiscard]] bool IsValid() const {
        return index != 0;
    }

    bool operator==(THandle<Type> const& Other) const {
        return index == Other.index && generation == Other.generation;
    }
};