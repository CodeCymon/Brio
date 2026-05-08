#pragma once

#include <memory>

template<typename T>
using Unique = std::unique_ptr<T>;
