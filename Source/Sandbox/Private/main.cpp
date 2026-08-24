// Copyright (c) Simon Kirsch 2026.

#include "CommandList.h"
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

    Log::SetMinSeverity(Log::Severity::Info);

    Platform::Initialize();

    Window mainWindow;
    Window::Create(
        {1600, 900,"Brio Editor",true},
        mainWindow
    );
    mainWindow.OnCloseDelegate.BindObject(&engine, &Engine::Stop);

    RHI::Create(mainWindow.NativeData());

    // TODO: make OnResizeDelegate multicast
    mainWindow.OnResizeDelegate.BindObject(GDynamicRHI, &IDynamicRHI::OnResize);

    {
        /*
        RHITextureRef myImage = GDynamicRHI->CreateTexture(
           RHITextureDesc::Texture2D(PixelFormat::RGBA8_SRGB, {800, 450},
                                     TextureUsage::TransferSrc | TextureUsage::TransferDst),
           "firstCustomTexture");
           */

        while (engine.IsRunning()) {
            Platform::PollEvents();

            RHIFrameContext frame = GDynamicRHI->BeginFrame();
            ICommandList* cmdList = frame.cmdList;

            // cmdList->ClearImage(myImage, {1,0,1,1});
            cmdList->TransitionImage(frame.swapchainTexture, RHIResourceState::Undefined, RHIResourceState::TransferDst);
            cmdList->ClearImage(frame.swapchainTexture, {0,0,1,1});
            cmdList->TransitionImage(frame.swapchainTexture, RHIResourceState::TransferDst, RHIResourceState::Present);

            GDynamicRHI->EndFrame();
        }
    }

    RHI::Destroy();

    mainWindow.Close();

    Platform::Shutdown();

    return 0;
}

