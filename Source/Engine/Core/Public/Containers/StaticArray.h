// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"
#include "Log/Assert.h"

template<typename ElementType, u32 numElements>
class StaticArray {
public:
    using SizeType = u32;
private:
    ElementType data[numElements]{};

public:
    constexpr StaticArray() = default;

    constexpr StaticArray(ElementType const* ptr, SizeType count) {
        ASSERT(count > 0 && count <= numElements);
        for (SizeType i = 0; i < count; ++i)
            data[i] = ptr[i];
    }

    constexpr StaticArray(std::initializer_list<ElementType> list) {
        ASSERT(list.size() <= numElements);
        for (SizeType i = 0; i < list.size(); ++i)
            data[i] = list.begin()[i];
    }

    StaticArray(StaticArray const&) = default;

    StaticArray& operator=(std::initializer_list<ElementType> list) {
        ASSERT(list.size() <= numElements);
        for (SizeType i = 0; i < list.size(); ++i)
            data[i] = list.begin()[i];
    }

    StaticArray& operator=(StaticArray const&) = default;

public:
    StaticArray(StaticArray&&) = default;

    StaticArray& operator=(StaticArray&&) = default;

    ~StaticArray() = default;

public:
    [[nodiscard]] ElementType* Data() {
        return data;
    }

    [[nodiscard]] ElementType const* Data() const {
        return data;
    }

    [[nodiscard]] SizeType Size() const {
        return numElements;
    }

    [[nodiscard]] SizeType Capacity() const {
        return numElements;
    }

    [[nodiscard]] bool IsValidIndex(SizeType index) const {
        return index >= 0 && index < numElements;
    }

    [[nodiscard]] ElementType& operator[](SizeType index) {
        ASSERT(IsValidIndex(index));
        return data[index];
    }

    [[nodiscard]] ElementType const& operator[](SizeType index) const {
        ASSERT(IsValidIndex(index));
        return data[index];
    }

public:
    using Iterator = ElementType*;
    using ConstIterator = ElementType const*;

    constexpr Iterator begin() { return &data[0]; }
    constexpr Iterator end() { return &data[numElements]; }
    constexpr ConstIterator begin() const { return &data[0]; }
    constexpr ConstIterator end() const { return &data[numElements]; }
};

template<typename T, typename... U>
StaticArray(T, U...) -> StaticArray<T, 1 + sizeof...(U)>;