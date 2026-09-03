// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <atomic>

#include "CoreMinimal.h"
#include "RHIDefinitions.h"
#include "Memory/RefCountedPtr.h"

using RHITextureRef = TRefCountedPtr<class RHITexture>;

using RHIVertexShaderRef = TRefCountedPtr<class RHIVertexShader>;
using RHIPixelShaderRef = TRefCountedPtr<class RHIPixelShader>;
using RHIComputeShaderRef = TRefCountedPtr<class RHIComputeShader>;

class RHIResource {
public:
    RHIResource() = default;

    void AddRef() {
        refCount.fetch_add(1, std::memory_order_acquire);
    }
    void Release() {
        u32 prevRefs = refCount.fetch_sub(1, std::memory_order_release);
        if ((prevRefs - 1) == 0)
            delete this;
    }

protected:
    virtual ~RHIResource() = default;

private:
    std::atomic<u32> refCount {0};
};



struct RHITextureDesc {
    TextureDimension dimension = TextureDimension::Tex2D;
    PixelFormat format = PixelFormat::Unknown;
    Extent3D extent;
    u32 mipLevels = 1;
    u32 arrayLayers = 1;
    u32 sampleCount = 1;
    TextureUsage usage = TextureUsage::None;

    static RHITextureDesc Texture2D(PixelFormat format, Extent2D extent, TextureUsage usage) {
        RHITextureDesc desc;
        desc.dimension = TextureDimension::Tex2D;
        desc.format = format;
        desc.extent = {extent.width, extent.height, 1};
        desc.usage = usage;
        return desc;
    }
};

class RHITexture : public RHIResource {
public:
    [[nodiscard]] RHITextureDesc const& Desc() const { return desc; }

protected:
    explicit RHITexture(RHITextureDesc const& inDesc) : desc(inDesc) {}

    RHITextureDesc desc;
};



struct RHIShaderDesc {
    Array<u32> codeBytes;
};

class RHIShader : public RHIResource {
public:
    RHIShader() = delete;
    explicit RHIShader(ShaderStage stage) : stage(stage) {}

    [[nodiscard]] ShaderStage GetShaderStage() const { return stage; }

private:
    ShaderStage stage;
};

class RHIVertexShader : public RHIShader {
public:
    RHIVertexShader() : RHIShader(ShaderStage::Vertex) {}
};

class RHIPixelShader : public RHIShader {
public:
    RHIPixelShader() : RHIShader(ShaderStage::Pixel) {}
};

class RHIComputeShader : public RHIShader {
public:
    RHIComputeShader() : RHIShader(ShaderStage::Compute) {}
};