#include "Engine.h"

#include "Platform.h"
#include "TimeManager.h"

Engine::Engine()
    : Platform(NewUnique<class Platform>())
    , TimeManager(NewUnique<class TimeManager>())
{}

Engine::~Engine() = default;

bool Engine::Initialize(FConfig const &Config) {
    if (!Platform->Initialize({Config.width, Config.height, Config.title, true}))
        return false;
    Platform->OnCloseDelegate.Add(this, &Engine::Quit);

    TimeManager->Initialize();

    return true;
}

void Engine::Shutdown() {
    Platform->Shutdown();
}

void Engine::Run() {

    while (bRunning) {
        f32 DeltaTme = TimeManager->DeltaTime();
        Platform->PollEvents();
        Tick(DeltaTme);
        Render();
    }
}

void Engine::Tick(f32 DeltaTime) {
}

void Engine::Render() {}

void Engine::Quit() {
    bRunning = false;
}
