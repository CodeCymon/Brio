#include "Engine.h"

#include "Platform.h"
#include "TimeManager.h"

#include "DynamicRHI.h"

#include <thread>


Engine::Engine()
    : platform(NewUnique<Platform>())
    , timeManager(NewUnique<TimeManager>())
{}

Engine::~Engine() = default;

bool Engine::Initialize(FConfig const &Config) {
    if (!platform->Initialize({Config.width, Config.height, Config.title, true}))
        return false;
    platform->OnCloseDelegate.Add(this, &Engine::OnQuit);

    timeManager->Initialize();

    RHI::Create(platform->NativeHandle());
    platform->OnResizeDelegate.Add(GDynamicRHI, &IDynamicRHI::OnResize);

    platform->OnFocusChangeDelegate.Add(this, &Engine::OnFocused);

    return true;
}

void Engine::Shutdown() {

    RHI::Destroy();

    platform->Shutdown();
}

void Engine::Run() {

    f32 DeltaTime = 0;
    while (bRunning) {
        DeltaTime = timeManager->DeltaTime();
        platform->PollEvents();
        Tick(DeltaTime);
        Render();

        if (bThrottleMainLoop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }
}

void Engine::Tick(f32 DeltaTime) {
}

void Engine::Render() {
    FRHIFrameContext frame = GDynamicRHI->BeginFrame();
    GDynamicRHI->EndFrame();
}

void Engine::OnQuit() {
    bRunning = false;
}

void Engine::OnFocused(bool bFocused) {
    bThrottleMainLoop = !bFocused;
}
