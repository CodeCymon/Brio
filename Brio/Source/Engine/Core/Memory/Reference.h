#pragma once
#include <memory>

template<typename T>
using TRef = std::unique_ptr<T>;