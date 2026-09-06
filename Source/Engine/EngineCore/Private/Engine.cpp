// Copyright (c) Simon Kirsch 2026.

#include "Engine.h"

#include "CommandList.h"
#include "DynamicRHI.h"
#include "PixelShaderUtils.h"
#include "Platform.h"
#include "RHI.h"
#include "ShaderCompiler.h"
#include "Math/Vector3.h"

DEFINE_GLOBAL_SHADER(VisualizeUVPS, "VisualizeUV.slang", "PSMain", ShaderStage::Pixel);

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

void Engine::Run() {
    ShaderMapRef<VisualizeUVPS> pixelShader = globalShaderMap.Get<VisualizeUVPS>();
    RHIGraphicsPipelineRef trianglePipeline = PixelShaderUtils::CreateFullscreenPipeline(&globalShaderMap, pixelShader.Get(), 0, 0, PixelFormat::RGBA8_SRGB);

    while (IsRunning()) {
        Platform::PollEvents();

        RHIFrameContext frame = RHIBeginFrame();
        ICommandList& cmdList = *frame.cmdList;

        cmdList.TransitionImage(frame.backBuffer, RHIResourceState::Undefined, RHIResourceState::ColorAttachment);

        cmdList.BeginRendering(frame.backBuffer);
        cmdList.SetViewport({0,0}, frame.backBuffer->Desc().extent);
        cmdList.SetScissor({0,0}, frame.backBuffer->Desc().extent);

        PixelShaderUtils::DrawFullscreenTriangle(cmdList, trianglePipeline, nullptr);

        cmdList.EndRendering();

        cmdList.TransitionImage(frame.backBuffer, RHIResourceState::ColorAttachment, RHIResourceState::Present);

        RHIEndFrame();
    }
}

void Engine::Shutdown() {
    globalShaderMap.ReleaseResources();

    RHI::Destroy();

    mainWindow.Close();

    ShaderCompiler::Shutdown();

    Platform::Shutdown();

    Log::Shutdown();
}
