// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <atomic>

#include "CoreMinimal.h"


class RHIResource {
public:
    virtual ~RHIResource() = default;

    void AddRef() {
        ++refCount;
    }
    void Release() {
        if (--refCount == 0) {
            OnRefCountZero();
        }
    }

    u32 GetRefCount() const {
        return refCount;
    }

protected:
    RHIResource() = default;

    virtual void OnRefCountZero() = 0;

private:
    std::atomic<u32> refCount {0};
};