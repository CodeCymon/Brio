#pragma once

#include "Log/Assert.h"
#include "Types/CoreTypes.h"
#include "Memory/MemoryUtils.h"

#include <initializer_list>

template<typename ElementType>
class TArray {
private:
    ElementType* ArrayData { nullptr };
    usize ArrayNum { 0 };
    usize ArrayMax { 0 };

private:
    void Reallocate(usize InCapacity) {
        ElementType* NewData = new ElementType[InCapacity];
        for (usize i = 0; i < ArrayNum; ++i) {
            NewData[i] = MoveTemp(ArrayData[i]);
        }
        delete[] ArrayData;
        ArrayData = NewData;
        ArrayMax = InCapacity;
    }

    void EnsureCapacity() {
        if (ArrayNum >= ArrayMax)
            Reallocate(ArrayMax == 0 ? 4 : ArrayMax * 2);
    }

public:
    TArray() = default;

    TArray(TArray const& Other) {
        if (Other.ArrayNum != 0) {
            Reallocate(Other.ArrayNum);
            for (usize i = 0; i < Other.ArrayNum; ++i) {
                ArrayData[i] = Other[i];
            }
            ArrayNum = Other.ArrayNum;
        }
    }

    TArray(TArray&& Other) noexcept 
    : ArrayData(Other.ArrayData)
    , ArrayNum(Other.ArrayNum)
    , ArrayMax(Other.ArrayMax) 
    {
        Other.ArrayData = nullptr;
        Other.ArrayNum = 0;
        Other.ArrayMax = 0;
    }

    TArray& operator=(TArray const& Other) {
        if (this == &Other)
            return *this;

        TArray TempArray(Other);
        Swap(ArrayData, TempArray.ArrayData);
        Swap(ArrayNum, TempArray.ArrayNum);
        Swap(ArrayMax, TempArray.ArrayMax);
        return *this;
    }

    TArray& operator=(TArray&& Other) noexcept {
        if (this == &Other)
            return *this;

        delete[] ArrayData;
        ArrayData = Other.ArrayData;
        ArrayNum = Other.ArrayNum;
        ArrayMax = Other.ArrayMax;

        Other.ArrayData = nullptr;
        Other.ArrayNum = 0;
        Other.ArrayMax = 0;

        return *this;
    }

    ~TArray() {
        delete[] ArrayData;
    }

    TArray(usize Count) {
        ASSERT(Count != 0);

        Reallocate(Count);
        ArrayNum = Count;
    }

    TArray(ElementType const* Pointer, usize Count) {
        ASSERT(Pointer != nullptr && Count != 0);

        Reallocate(Count);
        for (usize i = 0; i < Count; ++i) {
            ArrayData[i] = Pointer[i];
        }
        ArrayNum = Count;
    }

    TArray(std::initializer_list<ElementType> InitList) {
        Reallocate(InitList.size());
        for (usize i = 0; i < InitList.size(); ++i) {
            ArrayData[i] = InitList.begin()[i];
        }
        ArrayNum = InitList.size();
    }

    usize Num() const { return ArrayNum; }
    usize Max() const { return ArrayMax; }

    ElementType* Data() { return ArrayData; }
    ElementType const* Data() const { return ArrayData; }

    ElementType& operator[](usize Index) {
        ASSERT(Index < ArrayNum);
        return ArrayData[Index];
    }

    ElementType const& operator[](usize Index) const {
        ASSERT(Index < ArrayNum);
        return ArrayData[Index];
    }

    bool IsEmpty() const {
        return ArrayNum == 0;
    }

    void Add(ElementType const& Element) {
        EnsureCapacity();
        ArrayData[ArrayNum++] = Element;
    }

    void Add(ElementType&& Element) {
        EnsureCapacity();
        ArrayData[ArrayNum++] = MoveTemp(Element);
    }

    template<typename... ArgsType>
    ElementType& Emplace(ArgsType&&... Args) {
        EnsureCapacity();
        ArrayData[ArrayNum] = ElementType(Forward<ArgsType>(Args)...);
        return ArrayData[ArrayNum++];
    }

    void Insert(usize Index, ElementType const& Element) {
        ASSERT(Index < ArrayNum);
        EnsureCapacity();
        for (usize i = ArrayNum; i > Index; --i) {
            ArrayData[i] = MoveTemp(ArrayData[i - 1]);
        }
        ArrayData[Index] = Element;
        ++ArrayNum;
    }

    void RemoveAt(usize Index) {
        ASSERT(Index < ArrayNum);
        for (usize i = Index; i+1 < ArrayNum; ++i) {
            ArrayData[i] = MoveTemp(ArrayData[i+1]);
        }
        --ArrayNum;
    }

    void Pop() {
        ASSERT(ArrayNum != 0);
        --ArrayNum;
    }

    void Push(ElementType const& Element) {
        Add(Element);
    }

    void Push(ElementType&& Element) {
        Add(Element);
    }

    void Empty() {
        ArrayNum = 0;
    }

    void Reserve(usize Capacity) {
        if (Capacity > ArrayMax)
            Reallocate(Capacity);
    }

    void Resize(usize NewSize) {
        if (NewSize > ArrayMax)
            Reallocate(NewSize);
        ArrayNum = NewSize;
    }

    using Iterator = ElementType*;
    using ConstIterator = ElementType const*;

    Iterator begin() { return &ArrayData[0]; }
    Iterator end() { return &ArrayData[ArrayNum]; }
    ConstIterator begin() const { return &ArrayData[0]; }
    ConstIterator end() const { return &ArrayData[ArrayNum]; }
};
