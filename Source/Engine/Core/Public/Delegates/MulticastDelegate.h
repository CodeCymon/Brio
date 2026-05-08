#pragma once

#include <cstring>

#include "Types/Handle.h"

using FDelegateHandle = THandle<struct FInternalDelegateHandleType>;

template<typename Signature>
class TMulticastDelegate;

template<typename... Args>
class TMulticastDelegate<void(Args...)> {
public:
    template<typename T>
    FDelegateHandle Add(T* Instance, void(T::*Method)(Args...)) {
        u32 Slot = Count++;
        static_assert(sizeof(Method) <= sizeof(MethodPointers[0]));
        std::memcpy(MethodPointers[Slot], &Method, sizeof(Method));
        Listeners[Slot] = { Instance, &Thunk<T> };
        Handles[Slot] = { NextIndex++, Generation };
        return Handles[Slot];
    }

    void Remove(FDelegateHandle Handle) {
        for (u32 i = 0; i < Count; i++) {
            if (Handles[i] != Handle)
                continue;

            Listeners[i] = Listeners[Count - 1];
            Handles[i] = Handles[Count - 1];
            std::memcpy(MethodPointers[i], MethodPointers[Count - 1], sizeof(MethodPointers[0]));
            --Count;
            ++Generation;
            return;
        }
    }

    void Broadcast(Args... args) const {
        for (u32 i = 0; i < Count; i++) {
            Listeners[i].Invoke(Listeners[i].Instance, MethodPointers[i], args...);
        }
    }

private:
    template<typename T>
    static void Thunk(void* Instance, void const* MethodPointer, Args... args) {
        void(T::*Method)(Args...);
        std::memcpy(&Method, MethodPointer, sizeof(Method));
        (static_cast<T*>(Instance)->*Method)(args...);
    }

    struct Slot {
        void* Instance {nullptr};
        void(*Invoke)(void*, void const*, Args...) {nullptr};
    };

    static constexpr u32 MAX_LISTENERS = 16;
    Slot                Listeners[MAX_LISTENERS] {};
    alignas(void*) char MethodPointers[MAX_LISTENERS][sizeof(void*) * 2] {};
    FDelegateHandle     Handles[MAX_LISTENERS] {};
    u32 Count {0};
    u32 Generation {0};
    u32 NextIndex {1};
};