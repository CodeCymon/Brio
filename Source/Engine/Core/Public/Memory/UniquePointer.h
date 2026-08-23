// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <memory>

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
static UniquePtr<T> MakeUnique() {
    return std::make_unique<T>();
}
