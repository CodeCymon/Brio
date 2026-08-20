// Copyright (c) Simon Kirsch 2026.

#pragma once

#include <initializer_list>

#include "Core/CoreTypes.h"
#include "Log/Assert.h"
#include "Memory/Utilities.h"

template<typename ElementType>
class Array {
private:
    ElementType* data {nullptr};
    i32 capacity{};
    i32 count{};

public:
    [[nodiscard]] constexpr Array()
    : capacity(0)
    , count(0)
    {}

    [[nodiscard]] explicit constexpr Array(u32 initialCount) {
        data = Allocate(initialCount);
        capacity = initialCount;
        count = initialCount;
    }

    [[nodiscard]] Array(ElementType const* ptr, i32 numElements) {
        CopyToEmpty(ptr, numElements);
    }

    [[nodiscard]] Array(std::initializer_list<ElementType> list) {
        CopyToEmpty(list.begin(), static_cast<i32>(list.size()));
    }

    [[nodiscard]] Array(Array const& other) {
        CopyToEmpty(other.Data(), other.Size());
    }

    Array& operator=(std::initializer_list<ElementType> list) {
        DestroyAndFree();
        CopyToEmpty(list.begin(), static_cast<i32>(list.size()));
        return *this;
    }

    Array& operator=(Array const& other) {
        if (this != &other) {
            DestroyAndFree();
            CopyToEmpty(other.Data(), other.Size());
        }
        return *this;
    }

public:
    [[nodiscard]] Array(Array&& other) noexcept
    : data(other.data)
    , capacity(other.capacity)
    , count(other.count) {
        other.data = nullptr;
        other.count = 0;
        other.capacity = 0;
    }

    Array& operator=(Array&& other)  noexcept {
        if (this != &other) {
            DestroyAndFree();
            data = other.data;
            count = other.count;
            capacity = other.capacity;
            other.data = nullptr;
            other.count = 0;
            other.capacity = 0;
        }
        return *this;
    }

    ~Array() {
        DestroyAndFree();
    }

public:
    [[nodiscard]] ElementType* Data() {
        return data;
    }

    [[nodiscard]] ElementType const* Data() const {
        return data;
    }

    [[nodiscard]] i32 Size() const {
        return count;
    }

    [[nodiscard]] i32 Capacity() const {
        return capacity;
    }

    [[nodiscard]] bool IsValidIndex(i32 index) const {
        return index >= 0 && index < count;
    }

    [[nodiscard]] bool IsEmpty() const {
        return count == 0;
    }

    [[nodiscard]] ElementType& operator[](i32 index) {
        ASSERT(IsValidIndex(index));
        return Data()[index];
    }

    [[nodiscard]] ElementType const& operator[](i32 index) const {
        ASSERT(IsValidIndex(index));
        return Data()[index];
    }

public:
    void Add(ElementType const& element) {
        if (count >= capacity) {
            ElementType copy = element;
            Grow(capacity == 0 ? 4 : capacity * 2);
            ConstructAt(data + count++, Move(copy));
            return;
        }
        ConstructAt(data + count++, element);
    }

    void Add(ElementType&& element) {
        EnsureCapacity();
        ConstructAt(data + count++, Move(element));
    }

    template<typename... Args>
    ElementType& Emplace(Args&&... args) {
        EnsureCapacity();
        ElementType* slot = ConstructAt(data + count, std::forward<Args>(args)...);
        ++count;
        return *slot;
    }

    void InsertAt(ElementType const& element, i32 index) {
        ASSERT(index >= 0 && index <= count);
        ElementType copy = element;
        EnsureCapacity();

        if (index < count) {
            ConstructAt(data + count, MoveIfPossible(data[count-1]));
            for (i32 i = count-1; i > index; --i)
                data[i] = MoveIfPossible(data[i-1]);
            data[index] = Move(copy);
        } else {
          ConstructAt(data + count, Move(copy));
        }
        ++count;
    }

    void RemoveAt(i32 index) {
        ASSERT(IsValidIndex(index));
        for (i32 i = index; i+1 < count; i++)
            data[i] = MoveIfPossible(data[i + 1]);
        --count;
        Destroy(data[count]);
    }

    void Push(ElementType const& element) {
        Add(element);
    }

    void Push(ElementType&& element) {
        Add(Move(element));
    }

    [[nodiscard]] ElementType Pop() {
        ASSERT(count > 0);
        ElementType result = MoveIfPossible(data[count-1]);
        --count;
        Destroy(data[count]);
        return result;
    }

    void Clear() {
        DestroyRange(0, count);
        count = 0;
    }

    void Reserve(i32 newCapacity) {
        if (capacity < newCapacity)
            Grow(newCapacity);
    }

    void Resize(i32 newCount) {
        ASSERT(newCount >= 0);

        if (newCount < count) {
            DestroyRange(newCount, count);
            count = newCount;
            return;
        }

        if (newCount > capacity)
            Grow(newCount);

        if (newCount > count) {
            for (i32 i = count; i < newCount; ++i)
                ConstructAt(data + i);
            count = newCount;
        }
    }

private:
    static ElementType* Allocate(i32 n) {
        if (n == 0)
            return nullptr;
        return static_cast<ElementType*>(::operator new(static_cast<usize>(n) * sizeof(ElementType)));
    }

    static void Deallocate(ElementType* ptr) {
        ::operator delete(ptr);
    }

    template<typename... Args>
    static ElementType* ConstructAt(ElementType* ptr, Args&&... args) {
        return ::new (static_cast<void*>(ptr)) ElementType(std::forward<Args>(args)...);
    }

    static void Destroy(ElementType& element) {
        element.~ElementType();
    }

    void DestroyRange(i32 first, i32 last) {
        for (i32 i = first; i < last; i++)
            Destroy(data[i]);
    }

    void DestroyAndFree() {
        DestroyRange(0, count);
        Deallocate(data);
        data = nullptr;
        count = 0;
        capacity = 0;
    }

    void CopyToEmpty(ElementType const* source, i32 numElements) {
        data = Allocate(numElements);
        capacity = numElements;
        count = numElements;
        for (i32 i = 0; i < numElements; i++)
            ConstructAt(data + i, source[i]);
    }

    void Grow(i32 newCapacity) {
        ElementType* newData = Allocate(newCapacity);
        for (i32 i = 0; i < count; i++)
            ConstructAt(newData + i, MoveIfPossible(data[i]));
        DestroyRange(0, count);
        Deallocate(data);
        data = newData;
        capacity = newCapacity;
    }

    void EnsureCapacity() {
        if (count >= capacity)
            Grow(capacity == 0 ? 4 : capacity * 2);
    }

public:
    using Iterator = ElementType*;
    using ConstIterator = ElementType const*;

    Iterator begin() { return &data[0]; }
    Iterator end() { return &data[count]; }
    ConstIterator begin() const { return &data[0]; }
    ConstIterator end() const { return &data[count]; }
};
