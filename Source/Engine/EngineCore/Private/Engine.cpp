// Copyright (c) Simon Kirsch 2026.

#include "Engine.h"

#include "CommandList.h"
#include "DynamicRHI.h"
#include "Platform.h"
#include "RHI.h"
#include "ShaderCompiler.h"

bool Engine::Initialize() {
    if (Log::Initialize() == false)
        return false;
    Log::SetMinSeverity(Log::Severity::Debug);

    if (Platform::Initialize() == false)
        return false;

    if (ShaderCompiler::Initialize() == false)
        return false;

    if (Window::Create({1600,900,"Brio", true}, mainWindow) == false)
        return false;
    mainWindow.OnCloseDelegate.BindObject(this, &Engine::Stop);

    if (RHI::Create(mainWindow.NativeData(), mainWindow.Extent()) == false)
        return false;
    mainWindow.OnResizeDelegate.BindObject(GDynamicRHI, &IDynamicRHI::OnResize);

    bRunning = true;

    return true;

}

void Engine::Run() const {
    // TODO: ~ remove begin
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
    // TODO: ~ remove end

    while (IsRunning()) {
        Platform::PollEvents();

        // TODO: ~ remove begin
        RHIFrameContext frame = RHIBeginFrame();
        ICommandList& cmdList = *frame.cmdList;

        cmdList.TransitionImage(frame.backBuffer, RHIResourceState::Undefined, RHIResourceState::ColorAttachment);

        cmdList.BindPipeline(trianglePipeline);
        cmdList.BeginRendering(frame.backBuffer);
        cmdList.EndRendering();

        cmdList.TransitionImage(frame.backBuffer, RHIResourceState::ColorAttachment, RHIResourceState::Present);

        RHIEndFrame();
        // TODO: ~ remove end
    }
}

void Engine::Shutdown() {
    RHI::Destroy();

    mainWindow.Close();

    ShaderCompiler::Shutdown();

    Platform::Shutdown();

    Log::Shutdown();
}
