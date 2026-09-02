// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <cstddef>

template<typename T>
class TRefCountedPtr {
public:
    TRefCountedPtr() = default;
    explicit TRefCountedPtr(std::nullptr_t) {}

    explicit TRefCountedPtr(T* inPtr) : ptr(inPtr) {
        if (ptr) ptr->AddRef();
    }

    TRefCountedPtr(TRefCountedPtr const& o) {
        ptr = o.ptr;
        if (ptr) ptr->AddRef();
    }

    TRefCountedPtr(TRefCountedPtr&& o) noexcept : ptr(o.ptr) {
        o.ptr = nullptr;
    }

    ~TRefCountedPtr() {
        if (ptr) ptr->Release();
    }

    TRefCountedPtr& operator=(TRefCountedPtr const& o) {
        if (this != &o) {
            if (o.ptr) o.ptr->AddRef();
            if (ptr) ptr->Release();
            ptr = o.ptr;
        }
        return *this;
    }

    TRefCountedPtr& operator=(TRefCountedPtr&& o) noexcept {
        if (this != &o) {
            if (ptr) ptr->Release();
            ptr = o.ptr;
            o.ptr = nullptr;
        }
        return *this;
    }


public:
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }

    operator T*() const { return ptr; }

    [[nodiscard]] T* Get() const { return ptr; }

    explicit operator bool() const { return ptr != nullptr; }
    bool operator==(TRefCountedPtr const& o) const { return ptr == o.ptr; }

private:
    T* ptr = nullptr;
};