#pragma once

#include "Memory/MemoryUtils.h"
#include "Types/CoreTypes.h"

template <typename ElementType, u32 Size>
class TFixedStack {
private:
    u32 Top { 0 };
    ElementType Data[Size] {};

public:
    bool IsEmpty() const {
        return Top == 0;
    }

    bool IsFull() const {
        return Top >= Size;
    }

    void Push(ElementType const& Element) {
        Data[Top] = Element;
        ++Top;
    }

    void Push(ElementType&& Element) {
        Data[Top] = MoveTemp(Element);
        ++Top;
    }

    ElementType& Pop() {
        return Data[--Top];
    }
};
