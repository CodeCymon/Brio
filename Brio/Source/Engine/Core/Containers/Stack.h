#pragma once
#include "Common/Types.h"
#include "Core/Log/Logger.h"

template<typename T, u32 N>
struct TStaticStack
{
private:
    i32 top {-1};
    T data [N] {};

public:
    [[nodiscard]]
    bool isEmpty() const
    {
        return top < 0;
    }

    [[nodiscard]]
    bool isFull() const
    {
        return top >= static_cast<i32>(N) - 1;
    }

    void push(T resource)
    {
        if (isFull())
        {
            LOG_FATAL(LogCore, "Stack Overflow!");
            return;
        }
        data[++top] = resource;
    }

    [[nodiscard]]
    T pop()
    {
        if (isEmpty())
        {
            LOG_FATAL(LogCore, "Stack Underflow!");
            return T{};
        }
        return data[top--];
    }
};
