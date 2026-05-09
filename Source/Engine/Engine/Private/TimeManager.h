#pragma once

#include <chrono>

#include "Types/CoreTypes.h"


class TimeManager {
public:
    bool Initialize();
    f32 DeltaTime();

private:
    std::chrono::steady_clock::time_point LastTime {};
};

