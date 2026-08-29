// Copyright (c) Simon Kirsch 2026.

#include <chrono>

#include "CommandList.h"
#include "DynamicRHI.h"
#include "Platform.h"
#include "Window.h"
#include "RHI.h"
#include "ShaderCompiler.h"

class Engine {
public:
    void Stop() {
        bRunning = false;
    }

    [[nodiscard]] bool IsRunning() const {
        return bRunning;
    }

private:
    bool bRunning {true};
};

int main() {
    Engine engine;

    Log::SetMinSeverity(Log::Severity::Debug);

    Platform::Initialize();

    ShaderCompiler::Initialize();

    Window mainWindow;
    Window::Create(
        {1600, 900,"Brio Editor",true},
        mainWindow
    );
    mainWindow.OnCloseDelegate.BindObject(&engine, &Engine::Stop);

    RHI::Create(mainWindow.NativeData());

    mainWindow.OnResizeDelegate.BindObject(GDynamicRHI, &IDynamicRHI::OnResize);

    // TODO: remove
    auto shaderCompileResult = ShaderCompiler::CompileFromFile("short.slang", "CSMain", ShaderStage::Compute);

    {
        RHITextureRef myTexture = GDynamicRHI->CreateTexture(
           RHITextureDesc::Texture2D(PixelFormat::RGBA8_SRGB, {800, 450},
                                     TextureUsage::TransferSrc | TextureUsage::TransferDst),
           "firstCustomTexture");

        u64 totalNS = 0;
        u64 frameCount = 0;
        while (engine.IsRunning()) {
            auto start = std::chrono::high_resolution_clock::now();

            Platform::PollEvents();

            RHIFrameContext frame = GDynamicRHI->BeginFrame();
            ICommandList* cmdList = frame.cmdList;

            cmdList->TransitionImage(myTexture, RHIResourceState::Undefined, RHIResourceState::TransferDst);
            cmdList->ClearImage(myTexture, {0,0,1,1});
            cmdList->TransitionImage(myTexture, RHIResourceState::TransferDst, RHIResourceState::TransferSrc);

            cmdList->TransitionImage(frame.swapchainTexture, RHIResourceState::Undefined, RHIResourceState::TransferDst);
            cmdList->BlitImage(myTexture, frame.swapchainTexture);
            cmdList->TransitionImage(frame.swapchainTexture, RHIResourceState::TransferDst, RHIResourceState::Present);

            GDynamicRHI->EndFrame();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            totalNS += duration.count();
            ++frameCount;
        }

        LOG_INFO(LogTemp, "Average MS per frame was: {}", (static_cast<f64>(totalNS) / frameCount) / 1000000.0 );
    }

    RHI::Destroy();

    mainWindow.Close();

    ShaderCompiler::Shutdown();

    Platform::Shutdown();

    return 0;
}

