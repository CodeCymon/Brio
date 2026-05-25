#include "Renderer.h"

#include "DynamicRHI.h"


void Renderer::Initialize() {
    triangleVS = GDynamicRHI->CreateShader({
        .path = "Triangle.hlsl",
        .stage = EShaderStage::Vertex
    });

    trianglePS = GDynamicRHI->CreateShader({
        .path = "Triangle.hlsl",
        .stage = EShaderStage::Pixel
    });

    trianglePipeline = GDynamicRHI->CreateGraphicsPipeline({
        .vertexShader = triangleVS,
        .pixelShader = trianglePS
    });
}

void Renderer::Shutdown() {
    GDynamicRHI->WaitForIdle();

    GDynamicRHI->DestroyGraphicsPipeline(trianglePipeline);
    GDynamicRHI->DestroyShader(trianglePS);
    GDynamicRHI->DestroyShader(triangleVS);
}

void Renderer::RenderFrame() {
    FRHIFrameContext frame = GDynamicRHI->BeginFrame();

    GDynamicRHI->BeginRendering(frame.swapchainImage);

    FExtent2D extent = frame.swapchainImage->GetDesc().Extent();
    GDynamicRHI->SetViewport(0, 0, extent.width, extent.height);
    GDynamicRHI->SetScissor(0, 0, extent.width, extent.height);
    GDynamicRHI->BindPipeline(trianglePipeline);
    GDynamicRHI->Draw(3,0);

    GDynamicRHI->EndRendering(frame.swapchainImage);

    GDynamicRHI->EndFrame();
}
