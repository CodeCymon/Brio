#pragma once

#include "EngineAPI.h"
#include "Memory/References.h"
#include "Types/CoreTypes.h"


class Platform;
class TimeManager;

class ENGINE_API Engine {
public:
    struct FConfig {
        u32 width {1280};
        u32 height {720};
        char const* title {"BrioEngine"};
    };

    Engine();
    ~Engine();

    bool Initialize(FConfig const& Config);
    void Shutdown();

    void Run();

private:
    void Tick(f32 DeltaTime);
    void Render();

    void Quit();

private:
    Unique<class Platform> Platform;
    Unique<class TimeManager> TimeManager;

    bool bRunning {true};
};
