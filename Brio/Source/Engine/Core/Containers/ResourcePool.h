#pragma once
#include "Handle.h"
#include "Stack.h"
#include "Core/Asserts/Assert.h"


template<typename T, u32 N = 1024>
struct TResourcePool
{
private:
    struct Slot
    {
        T resource {};
        u32 generation {0};
        bool alive {false};
    };

    Slot slots[N];
    TStaticStack<u32, N> freeStack;
    u32 nextUnused {1}; // slot 0 reserved as null

public:
    [[nodiscard]]
    THandle<T> alloc(T resource)
    {
        u32 index;
        if (!freeStack.isEmpty())
            index = freeStack.pop();
        else if (nextUnused < N)
            index = nextUnused++;
        else
            ASSERT(false, "Resource Pool exhausted!");

        slots[index].resource = resource;
        slots[index].alive = true;
        return THandle<T>{ index, slots[index].generation };
    }

    [[nodiscard]]
    T& get(THandle<T> handle)
    {
        Slot& slot = slots[handle.index];
        ASSERT(slot.alive, "Accessing destroyed resource!");
        ASSERT(slot.generation == handle.generation, "Stale Handle - Resource was destroyed and slot reused!");
        return slot.resource;
    }

    void free(THandle<T> handle)
    {
        Slot& slot = slots[handle.index];
        ASSERT(slot.alive, "Double Free!");
        ASSERT(slot.generation == handle.generation, "Freeing with Stale Handle!");
        slot.alive = false;
        ++slot.generation;
        freeStack.push(handle.index);
    }
};
