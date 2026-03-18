#pragma once
#include "Common/Types.h"

template<typename T>
struct THandle
{
    u32 index       : 20 {0};
    u32 generation  : 12 {0};

    [[nodiscard]]
    bool isValid() const { return index != 0; }

    bool operator==(THandle<T> const& other) const
    {
        return index == other.index && generation == other.generation;
    }
};