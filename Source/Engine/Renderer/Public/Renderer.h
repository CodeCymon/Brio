#pragma once

#include "RendererAPI.h"

#include "RHIMinimal.h"

class RENDERER_API Renderer {
public:
    void Initialize();
    void Shutdown();

    void RenderFrame();

private:
    FRHIShaderRef triangleVS {nullptr};
    FRHIShaderRef trianglePS {nullptr};
    FRHIGraphicsPipelineRef trianglePipeline {nullptr};
};
