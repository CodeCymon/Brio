// Copyright (c) Simon Kirsch 2026.

#include "Engine.h"

#include "CommandList.h"
#include "DynamicRHI.h"
#include "PixelShaderUtils.h"
#include "Platform.h"
#include "RHI.h"
#include "ShaderCompiler.h"
#include "Math/Vector3.h"

DEFINE_GLOBAL_SHADER(SphereTracerPS, "SphereTracer.slang", "PSMain", ShaderStage::Pixel);

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
    struct SphereTracingConstants {
        f32 aspectRatio = 1.0f;
    };
    SphereTracingConstants sphereTracingConstants;
    ShaderMapRef<SphereTracerPS> tracingShader = globalShaderMap.Get<SphereTracerPS>();
    RHIGraphicsPipelineRef sphereTracingPipeline = PixelShaderUtils::CreateFullscreenPipeline(&globalShaderMap, tracingShader.Get(), 0, sizeof(SphereTracingConstants), PixelFormat::RGBA8_SRGB);

    while (IsRunning()) {
        Platform::PollEvents();

        RHIFrameContext frame = RHIBeginFrame();
        ICommandList& cmdList = *frame.cmdList;

        cmdList.TransitionImage(frame.backBuffer, RHIResourceState::Undefined, RHIResourceState::ColorAttachment);

        cmdList.BeginRendering(frame.backBuffer);
        cmdList.SetViewport({0,0}, frame.backBuffer->Desc().extent);
        cmdList.SetScissor({0,0}, frame.backBuffer->Desc().extent);

        sphereTracingConstants.aspectRatio = (f32)frame.backBuffer->Desc().extent.x / (f32)frame.backBuffer->Desc().extent.y;
        PixelShaderUtils::DrawFullscreenTriangle(cmdList, sphereTracingPipeline, &sphereTracingConstants);

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
