// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <initializer_list>

#include "Core/CoreTypes.h"
#include "Log/Assert.h"
#include "Memory/Utilities.h"

template<typename ElementType, i32 maxElements>
class BoundedArray {
private:
    ElementType data[maxElements]{};
    i32 count{};

public:
    [[nodiscard]] constexpr BoundedArray() = default;

    [[nodiscard]] constexpr BoundedArray(std::initializer_list<ElementType> list) {
        ASSERT(static_cast<i32>(list.size()) <= maxElements);
        for (auto const& element : list)
            data[count++] = element;
    }

public:
    [[nodiscard]] constexpr ElementType* Data() {
        return data;
    }

    [[nodiscard]] constexpr ElementType const* Data() const {
        return data;
    }

    [[nodiscard]] constexpr i32 Size() const {
        return count;
    }

    [[nodiscard]] constexpr i32 Capacity() const {
        return maxElements;
    }

    [[nodiscard]] constexpr bool IsValidIndex(i32 index) const {
        return index >= 0 && index < count;
    }

    [[nodiscard]] constexpr bool IsEmpty() const {
        return count == 0;
    }

    [[nodiscard]] constexpr bool IsFull() const {
        return count == Capacity();
    }

    [[nodiscard]] constexpr ElementType& operator[](i32 index) {
        ASSERT(IsValidIndex(index));
        return data[index];
    }

    [[nodiscard]] constexpr ElementType const& operator[](i32 index) const {
        ASSERT(IsValidIndex(index));
        return data[index];
    }

public:
    constexpr void Add(ElementType const& element) {
        ASSERT(count < Capacity());
        data[count++] = element;
    }

    constexpr void Add(ElementType&& element) {
        ASSERT(count < Capacity());
        data[count++] = Move(element);
    }

    template<typename... Args>
    constexpr ElementType& Emplace(Args&&... args) {
        ASSERT(count < Capacity());
        data[count] = ElementType(std::forward<Args>(args)...);
        return data[count++];
    }

    constexpr void Push(ElementType const& element) {
        Add(element);
    }

    constexpr void Push(ElementType&& element) {
        Add(Move(element));
    }

    constexpr ElementType Pop() {
        ASSERT(count > 0);
        ElementType result = MoveIfPossible(data[count-1]);
        --count;
        return result;
    }

    constexpr void RemoveAt(i32 index) {
        ASSERT(IsValidIndex(index));
        for (i32 i = index; i+1 < count; ++i)
            data[i] = MoveIfPossible(data[i+1]);
        --count;
    }

    constexpr void Clear() {
        count = 0;
    }

public:
    using Iterator = ElementType*;
    using ConstIterator = ElementType const*;

    constexpr Iterator begin() { return &data[0]; }
    constexpr Iterator end() { return &data[count]; }
    constexpr ConstIterator begin() const { return &data[0]; }
    constexpr ConstIterator end() const { return &data[count]; }
};

template<typename T, typename... U>
BoundedArray(T, U...) -> BoundedArray<T, 1 + sizeof...(U)>;