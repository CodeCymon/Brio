// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <cstring>

#include "Log/Assert.h"
#include "Log/Log.h"


template<typename Signature>
class Delegate;


// TODO: Lambda/Functor binding and ExecuteIfBound() for non-void return-types
template<typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)> {
public:
    template<typename T>
    void BindObject(T* instance, ReturnType (T::*func)(Args...)) {
        static_assert(sizeof(func) <= sizeof(pFunc));
        if (bBound) LOG_WARNING(LogCore, "Delegate is already bound! This warning may indicate a bug in your code. Check if rebinding is truly necessary and if so use Unbind() before binding the new function.");

        std::memcpy(pFunc, &func, sizeof(func));
        pInstance = instance;
        pInvoke = &InstanceThunk<T>;
        bBound = true;
    }

    template<typename T>
    void BindObject(T* instance, ReturnType (T::*func)(Args...) const) {
        static_assert(sizeof(func) <= sizeof(pFunc));
        if (bBound) LOG_WARNING(LogCore, "Delegate is already bound! This warning may indicate a bug in your code. Check if rebinding is truly necessary and if so use Unbind() before binding the new function.");

        std::memcpy(pFunc, &func, sizeof(func));
        pInstance = instance;
        pInvoke = &InstanceThunkConst<T>;
        bBound = true;
    }

    void BindStatic(ReturnType (*func)(Args...)) {
        static_assert(sizeof(func) <= sizeof(pFunc));
        if (bBound) LOG_WARNING(LogCore, "Delegate is already bound! This warning may indicate a bug in your code. Check if rebinding is truly necessary and if so use Unbind() before binding the new function.");

        std::memcpy(pFunc, &func, sizeof(func));
        pInstance = nullptr;
        pInvoke = &StaticThunk;
        bBound = true;
    }

    void Unbind() {
        if (bBound == false) {
            LOG_WARNING(LogCore, "Delegate::Unbind() called on already unbound delegate!");
            return;
        }
        pInstance = nullptr;
        pInvoke = nullptr;
        std::memset(pFunc, 0, sizeof(pFunc));
        bBound = false;
    }

    ReturnType Execute(Args&&... args) const {
        ASSERTM(bBound, "Delegate::Execute() called on unbound delegate! Check your binding if this delegate is expected to be bound at all times, otherwise prefer ExecuteIfBound() or check IsBound() before calling Execute().");
        return pInvoke(pInstance, pFunc, std::forward<Args>(args)...);
    }

    bool ExecuteIfBound(Args&&... args) const requires(std::is_void_v<ReturnType>) {
        if (bBound)
            pInvoke(pInstance, pFunc, std::forward<Args>(args)...);
        return bBound;
    }

    [[nodiscard]] bool IsBound() const {
        return bBound;
    }

private:
    template<typename T>
    static ReturnType InstanceThunk(void* instance, void const* function, Args... args) {
        ReturnType(T::*func)(Args...);
        std::memcpy(&func, function, sizeof(func));
        return (static_cast<T*>(instance)->*func)(args...);
    }

    template<typename T>
    static ReturnType InstanceThunkConst(void* instance, void const* function, Args... args) {
        ReturnType(T::*func)(Args...) const;
        std::memcpy(&func, function, sizeof(func));
        return (static_cast<T*>(instance)->*func)(args...);
    }

    static ReturnType StaticThunk(void* instance, void const* function, Args... args) {
        ReturnType(*func)(Args...);
        std::memcpy(&func, function, sizeof(func));
        return (*func)(args...);
    }

private:
    void* pInstance{nullptr};
    alignas(void*) u8 pFunc[2 * sizeof(void*)]{};
    ReturnType(*pInvoke)(void*, void const*, Args...){nullptr};
    bool bBound{false};
};
