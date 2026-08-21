// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "Core/CoreTypes.h"
#include "Log/Assert.h"
#include "Memory/Utilities.h"

template<typename ElementType>
class Queue {
public:
    using SizeType = u32;
private:
    ElementType* data {nullptr};
    SizeType capacity {};
    SizeType head {};
    SizeType count {};

public:
    [[nodiscard]] constexpr Queue()
    : capacity(0)
    , head(0)
    , count(0)
    {}

    [[nodiscard]] Queue(Queue const& other) {
        CopyFrom(other);
    }

    Queue& operator=(Queue const& other) {
        if (this != &other) {
            DestroyAndFree();
            CopyFrom(other);
        }
        return *this;
    }

    [[nodiscard]] Queue(Queue&& other) noexcept
    : data(other.data)
    , capacity(other.capacity)
    , head(other.head)
    , count(other.count) {
        other.data = nullptr;
        other.capacity = 0;
        other.head = 0;
        other.count = 0;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            DestroyAndFree();
            data = other.data;
            capacity = other.capacity;
            head = other.head;
            count = other.count;
            other.data = nullptr;
            other.capacity = 0;
            other.head = 0;
            other.count = 0;
        }
        return *this;
    }


    ~Queue() {
        DestroyAndFree();
    }

public:
    [[nodiscard]] SizeType Size() const {
        return count;
    }

    [[nodiscard]] SizeType Capacity() const {
        return capacity;
    }

    [[nodiscard]] bool IsEmpty() const {
        return count == 0;
    }

public:
    void Enqueue(ElementType const& element) {
        EnsureCapacity();
        ConstructAt(data + PhysicalIndex(count), element);
        ++count;
    }

    void Enqueue(ElementType&& element) {
        EnsureCapacity();
        ConstructAt(data + PhysicalIndex(count), Move(element));
        ++count;
    }

    void Dequeue(ElementType& outElement) {
        ASSERT(count > 0);
        outElement = Move(data[head]);
        Destroy(data[head]);
        AdvanceHead();
        --count;
    }

    [[nodiscard]] ElementType& Front() {
        ASSERT(count > 0);
        return data[head];
    }

    [[nodiscard]] ElementType const& Front() const {
        ASSERT(count > 0);
        return data[head];
    }

    [[nodiscard]] ElementType& Back() {
        ASSERT(count > 0);
        return data[PhysicalIndex(count-1)];
    }

    [[nodiscard]] ElementType const& Back() const {
        ASSERT(count > 0);
        return data[PhysicalIndex(count-1)];
    }

    void Clear() {
        DestroyLogicalRange(0, count);
        head = 0;
        count = 0;
    }

    void Reserve(SizeType newCapacity) {
        if (newCapacity > capacity)
            Grow(newCapacity);
    }

private:
    static ElementType* Allocate(SizeType n) {
        if (n == 0)
            return nullptr;
        return static_cast<ElementType*>(operator new(static_cast<usize>(n) * sizeof(ElementType)));
    }

    static void Deallocate(ElementType* ptr) {
        operator delete(ptr);
    }

    template<typename... Args>
    static ElementType* ConstructAt(ElementType* ptr, Args&&... args) {
        return new (static_cast<void*>(ptr)) ElementType(std::forward<Args>(args)...);
    }

    static void Destroy(ElementType& element) {
        element.~ElementType();
    }

private:

    [[nodiscard]] SizeType PhysicalIndex(SizeType logicalIndex) const {
        SizeType idx = head + logicalIndex;
        if (idx >= capacity)
            idx -= capacity;
        return idx;
    }

    void AdvanceHead() {
        ++head;
        if (head == capacity)
            head = 0;
    }

    void DestroyLogicalRange(SizeType first, SizeType last) {
        for (SizeType i = first; i < last; i++)
            Destroy(data[PhysicalIndex(i)]);
    }

    void DestroyAndFree() {
        DestroyLogicalRange(0, count);
        Deallocate(data);
        data = nullptr;
        capacity = 0;
        head = 0;
        count = 0;
    }

    void CopyFrom(Queue const& other) {
        data = Allocate(other.count);
        capacity = other.count;
        head = 0;
        count = other.count;
        for (SizeType i = 0; i < other.count; i++)
            ConstructAt(data + i, other.data[other.PhysicalIndex(i)]);
    }

    void Grow(SizeType newCapacity) {
        ElementType* newData = Allocate(newCapacity);
        for (SizeType i = 0; i < count; i++)
            ConstructAt(newData + i, MoveIfPossible(data[PhysicalIndex(i)]));
        DestroyLogicalRange(0, count);
        Deallocate(data);
        data = newData;
        capacity = newCapacity;
        head = 0;
    }

    void EnsureCapacity() {
        if (count >= capacity)
            Grow(capacity == 0 ? 4 : capacity * 2);
    }
};
