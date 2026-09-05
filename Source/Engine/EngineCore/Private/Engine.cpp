// Copyright (c) Simon Kirsch 2026.

#include "Engine.h"

#include "CommandList.h"
#include "DynamicRHI.h"
#include "Platform.h"
#include "RHI.h"
#include "ShaderCompiler.h"
#include "Math/Vector3.h"

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

    struct Vertex {
        Vec3 position;
        float padding;
        Vec3 color;
        float padding2;
    };
    Vertex vertices[3] = {
        Vertex{{0, -1, 0},0, {1,0,0}},
          Vertex{{-1, 1,  0},0, {0,1,0}},
          Vertex{{1, 1,  0},0, {0,0,1}}
    };

    RHIBufferRef triangleBuffer = RHICreateBuffer({3 * sizeof(Vertex), BufferUsage::TransferDst|BufferUsage::ShaderAddressable}, "vertexBuffer");
    {
        RHIMappedBufferRef stagingBuffer = RHICreateMappedBuffer({3 * sizeof(Vertex), BufferUsage::TransferSrc}, "vertexStagingBuffer");
        std::memcpy(stagingBuffer->MappedPointer(), vertices, 3 * sizeof(Vertex));
        RHIImmediateSubmit([&](ICommandList& cmdList) {
           cmdList.CopyBuffer(stagingBuffer->Buffer(), triangleBuffer, sizeof(vertices), 0, 0);
        });
    }

    struct PushConstants {
        u64 bufferAddress;
    } constants;
    constants.bufferAddress = triangleBuffer->GetGpuAddress();

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
        .pushConstantSize = sizeof(PushConstants),
        .pushConstantOffset = 0,
        .colorFormat = PixelFormat::RGBA8_SRGB // hardcoded as (preferred) swapchain format
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
        cmdList.PushConstants(trianglePipeline, ShaderStage::Vertex|ShaderStage::Pixel, 0, sizeof(PushConstants), &constants);
        cmdList.BeginRendering(frame.backBuffer);
        cmdList.SetViewport({0,0}, frame.backBuffer->Desc().extent);
        cmdList.SetScissor({0,0}, frame.backBuffer->Desc().extent);
        cmdList.Draw(3,1,0,0);
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
