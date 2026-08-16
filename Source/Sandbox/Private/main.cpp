// Copyright (c) Simon Kirsch 2026.

#include "Platform.h"
#include "Window.h"

struct Engine {
    bool running = true;

    void Stop() {
        running = false;
    }

    static void Foo() {
        LOG_INFO(LogTemp, "Hello from 'const Engine::Foo()'");
    }
};

int main() {
    Engine engine;

    Platform::Initialize();

    Window mainWindow;
    Window::Create(
        {1600, 900,"Brio Editor",true},
        mainWindow
    );

    mainWindow.OnCloseDelegate.BindObject(&engine, &Engine::Stop);

    while (engine.running) {
        Platform::PollEvents();
    }

    mainWindow.Close();

    Platform::Shutdown();

    return 0;
}

