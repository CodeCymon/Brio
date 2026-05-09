#pragma once

#include <memory>

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename... Args>
Unique<T> NewUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
