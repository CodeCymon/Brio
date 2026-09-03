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

    {
        auto vertexCompileResult = ShaderCompiler::CompileFromFile("triangle.slang", "VSMain");
        RHIVertexShaderRef vertexShader = RHICreateVertexShader({vertexCompileResult.byteCode});

        auto pixelCompileResult = ShaderCompiler::CompileFromFile("triangle.slang", "PSMain");
        RHIPixelShaderRef pixelShader = RHICreatePixelShader({pixelCompileResult.byteCode});

        RHIGraphicsPipelineDesc triDesc {
            .vertexShader = vertexShader,
            .pixelShader = pixelShader,
            .raster = {
                .fillMode = FillMode::Solid,
                .cullMode = CullMode::None,
                .bClampDepth = false,
            },
            .blend = {
                .blendMode = BlendMode::Opaque
            },
            .colorFormat = PixelFormat::RGBA8_SRGB
        };
        RHIGraphicsPipelineRef trianglePipeline = RHICreateGraphicsPipeline(triDesc);


        u64 totalNS = 0;
        u64 frameCount = 0;
        while (engine.IsRunning()) {
            auto start = std::chrono::high_resolution_clock::now();

            Platform::PollEvents();

            RHIFrameContext frame = RHIBeginFrame();
            ICommandList& cmdList = *frame.cmdList;

            cmdList.TransitionImage(frame.swapchainTexture, RHIResourceState::Undefined, RHIResourceState::ColorAttachment);

            cmdList.BindPipeline(trianglePipeline);
            cmdList.BeginRendering(frame.swapchainTexture);
            cmdList.EndRendering();

            cmdList.TransitionImage(frame.swapchainTexture, RHIResourceState::ColorAttachment, RHIResourceState::Present);

            RHIEndFrame();

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

