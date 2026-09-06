// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "RenderCoreAPI.h"

#include "CommandList.h"
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "Core/CoreTypes.h"

#include "Shader.h"

namespace PixelShaderUtils {
    RENDERCORE_API RHIGraphicsPipelineRef CreateFullscreenPipeline(
        GlobalShaderMap* globalShaderMap,
        RHIPixelShader* pixelShader,
        u32 pushConstantOffset, u32 pushConstantSize,
        PixelFormat pixelFormat,
        BlendMode blendMode = BlendMode::Opaque);

    RENDERCORE_API void DrawFullscreenTriangle(ICommandList& cmdList, RHIGraphicsPipeline* pipeline, const void* pushConstantData);
}

DEFINE_GLOBAL_SHADER(FullscreenTriangleVS, "FullscreenTriangle.slang", "VSMain", ShaderStage::Vertex);
