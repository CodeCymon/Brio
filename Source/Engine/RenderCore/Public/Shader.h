// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "ShaderCompiler.h"
#include "Containers/Map.h"

template<ShaderStage stage>
struct ShaderStageTrait;

template<>
struct ShaderStageTrait<ShaderStage::Vertex> {
    using Type = RHIVertexShader;
};

template<>
struct ShaderStageTrait<ShaderStage::Pixel> {
    using Type = RHIPixelShader;
};

#define DEFINE_GLOBAL_SHADER(ShaderName, SourcePath, EntryPoint, Stage) \
struct ShaderName { \
using RHIType = typename ShaderStageTrait<Stage>::Type; \
static constexpr ShaderStage stage = Stage; \
static constexpr std::string_view source = SourcePath; \
static constexpr std::string_view entryPoint = EntryPoint; \
}

template<typename TShader>
class ShaderMapRef {
public:
    using RHIType = TShader::RHIType;

    ShaderMapRef() = default;

    explicit ShaderMapRef(RHIType* shader) : shader(shader) {}

    RHIType* Get() const { return shader; }
    RHIType* operator->() const { return shader; }
    operator RHIType*() const { return shader; }

private:
    RHIType* shader;
};

class GlobalShaderMap {
public:
    template<typename Shader>
    ShaderMapRef<Shader> Get() {
        auto compilerResult = ShaderCompiler::CompileFromFile(Shader::source, Shader::entryPoint);

        const u64 byteLength = compilerResult.byteCode.Size() * sizeof(u32);
        const u32 byteHash = std::_Hash_bytes(compilerResult.byteCode.Data(), byteLength, 0);
        u64 shaderKey = (byteLength << 32) | byteHash;

        if constexpr (Shader::stage == ShaderStage::Vertex) {
            if (auto foundPtr = vertexShaderMap.Get(shaderKey))
                return ShaderMapRef<Shader>{static_cast<Shader::RHIType*>(foundPtr->Get())};

            auto shader = RHICreateVertexShader({compilerResult.byteCode});
            auto* result = shader.Get();
            vertexShaderMap.Insert(shaderKey, Move(shader));
            return ShaderMapRef<Shader>{result};
        }
        else if constexpr (Shader::stage == ShaderStage::Pixel) {
            if (auto foundPtr = pixelShaderMap.Get(shaderKey))
                return ShaderMapRef<Shader>{foundPtr->Get()};

            auto shader = RHICreatePixelShader({compilerResult.byteCode});
            auto* result = shader.Get();
            pixelShaderMap.Insert(shaderKey, Move(shader));
            return ShaderMapRef<Shader>{result};
        }
        else {
            ASSERT(false);
        }
    }

    void ReleaseResources() {
        vertexShaderMap.Clear();
        pixelShaderMap.Clear();
        computeShaderMap.Clear();
    }

private:
    Map<u64, RHIVertexShaderRef> vertexShaderMap;
    Map<u64, RHIPixelShaderRef> pixelShaderMap;
    Map<u64, RHIComputeShaderRef> computeShaderMap;
};