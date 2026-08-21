// Copyright (c) Simon Kirsch 2026.

#include "DynamicRHI.h"
#include "Platform.h"
#include "Window.h"
#include "RHI.h"

class Engine {
public:
    void Stop() {
        bRunning = false;
    }

    bool IsRunning() const {
        return bRunning;
    }

private:
    bool bRunning {true};
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

    RHI::Create(mainWindow.NativeData());

    mainWindow.OnResizeDelegate.BindObject(GDynamicRHI, &IDynamicRHI::OnResize);

    while (engine.IsRunning()) {
        Platform::PollEvents();

        RHIFrameContext frame = GDynamicRHI->BeginFrame();
        GDynamicRHI->EndFrame();
    }

    RHI::Destroy();

    mainWindow.Close();

    Platform::Shutdown();

    return 0;
}

