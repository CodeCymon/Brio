#include "TimeManager.h"

bool TimeManager::Initialize() {
    LastTime = std::chrono::steady_clock::now();
    return true;
}

f32 TimeManager::DeltaTime() {
    auto now = std::chrono::steady_clock::now();
    f32 DeltaTime = std::chrono::duration<f32>(now - LastTime).count();
    LastTime = now;
    return DeltaTime;
}
