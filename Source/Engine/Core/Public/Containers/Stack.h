// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "Containers/Array.h"

template<typename ElementType>
class Stack {
private:
    Array<ElementType> data;

public:
    [[nodiscard]] Stack()
        : data()
    {}

    [[nodiscard]] Stack(ElementType const* ptr, i32 numElements)
        : data(ptr, numElements)
    {}

    [[nodiscard]] Stack(std::initializer_list<ElementType> list)
        : data(list)
    {}

    [[nodiscard]] Stack(Stack const& other) = default;

    Stack& operator=(std::initializer_list<ElementType> list) {
        data = list;
        return *this;
    }

    Stack& operator=(Stack const& other) = default;

public:
    [[nodiscard]] Stack(Stack&& other) noexcept = default;

    Stack& operator=(Stack&& other) noexcept = default;

    ~Stack() = default;

public:
    [[nodiscard]] i32 Size() const {
        return data.Size();
    }

    [[nodiscard]] i32 Capacity() const {
        return data.Capacity();
    }

    [[nodiscard]] bool IsEmpty() const {
        return data.IsEmpty();
    }

public:
    void Push(ElementType const& element) {
        data.Push(element);
    }

    void Push(ElementType&& element) {
        data.Push(Move(element));
    }

    [[nodiscard]] ElementType Pop() {
        return data.Pop();
    }

    [[nodiscard]] ElementType& Top() {
        return data[data.Size()-1];
    }

    [[nodiscard]] ElementType const& Peek() const {
        return data[data.Size()-1];
    }

    void Clear() {
        data.Clear();
    }

    void Reserve(i32 capacity) {
        data.Reserve(capacity);
    }
};