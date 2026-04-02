#pragma once
#include <vector>

template <typename T>
using TArray = std::vector<T>;


#include <array>
template<typename T, usize N>
using TStaticArray = std::array<T, N>;