// Copyright (c) Simon Kirsch 2026.

#include "PixelShaderUtils.h"

#include "CommandList.h"
#include "DynamicRHI.h"
#include "RHIResources.h"

namespace PixelShaderUtils {
    RHIGraphicsPipelineRef CreateFullscreenPipeline(GlobalShaderMap* globalShaderMap, RHIPixelShader* pixelShader, u32 pushConstantOffset, u32 pushConstantSize, PixelFormat pixelFormat, BlendMode blendMode) {
        auto vertexShader = globalShaderMap->Get<FullscreenTriangleVS>();
        RHIGraphicsPipelineDesc desc {
            .vertexShader = vertexShader.Get(),
            .pixelShader = pixelShader,
            .raster = {
                .fillMode = FillMode::Solid,
                .cullMode = CullMode::None,
                .bClampDepth = false,
            },
            .blend = {
                .blendMode = blendMode,
            },
            .pushConstantSize = pushConstantSize,
            .pushConstantOffset = pushConstantOffset,
            .colorFormat = pixelFormat,
            .depthFormat = PixelFormat::Unknown,
        };
        return RHICreateGraphicsPipeline(desc);
    }

    void DrawFullscreenTriangle(ICommandList& cmdList, RHIGraphicsPipeline* pipeline, const void* pushConstantData) {
        cmdList.BindPipeline(pipeline);
        if (pipeline->Desc().pushConstantSize > 0 && pushConstantData)
            cmdList.PushConstants(pipeline, ShaderStage::Pixel|ShaderStage::Vertex, pipeline->Desc().pushConstantOffset, pipeline->Desc().pushConstantSize, pushConstantData);
        cmdList.Draw(3,1,0,0);
    }
}
