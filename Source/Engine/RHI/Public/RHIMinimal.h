#pragma once

#include "Log/Log.h"

DEFINE_LOG_CATEGORY_MODULE(LogRHI);

class FRHITexture;
class FRHIShader;
class FRHIGraphicsPipeline;

using FRHITextureRef = FRHITexture*;
using FRHIShaderRef = FRHIShader*;
using FRHIGraphicsPipelineRef = FRHIGraphicsPipeline*;