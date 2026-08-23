// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <atomic>

#include "CoreMinimal.h"


class RHIResource {
public:
    virtual ~RHIResource() = default;
    void AddRef() { ++refCount; }
    void Release() { if (--refCount == 0) OnRefCountZero(); }

    virtual void Destroy() = 0;

protected:
    RHIResource() = default;
    virtual void OnRefCountZero() = 0;

private:
    std::atomic<u32> refCount {0};
};