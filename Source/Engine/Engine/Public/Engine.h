#pragma once

#include "EngineAPI.h"
#include "Memory/References.h"
#include "Types/CoreTypes.h"


class Renderer;
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

    void OnQuit();
    void OnFocused(bool bFocused);

private:
    Unique<Platform> platform;
    Unique<TimeManager> timeManager;
    Unique<Renderer> renderer;

    bool bRunning {true};
    bool bThrottleMainLoop {false};
};
