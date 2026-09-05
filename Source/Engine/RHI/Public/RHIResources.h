// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <atomic>

#include "Containers/Array.h"
#include "Memory/RefCountedPtr.h"
#include "Math/IntPoint.h"

#include "RHIDefinitions.h"
#include "RHIResources.h"

using RHITextureRef = TRefCountedPtr<class RHITexture>;

using RHIBufferRef = TRefCountedPtr<class RHIBuffer>;
using RHIMappedBufferRef = TRefCountedPtr<class RHIMappedBuffer>;

using RHIVertexShaderRef = TRefCountedPtr<class RHIVertexShader>;
using RHIPixelShaderRef = TRefCountedPtr<class RHIPixelShader>;
using RHIComputeShaderRef = TRefCountedPtr<class RHIComputeShader>;

using RHIGraphicsPipelineRef = TRefCountedPtr<class RHIGraphicsPipeline>;

//  -   -   -   -   -   -   -
//  Base Resource
//  -   -   -   -   -   -   -

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

//  -   -   -   -   -   -   -
//  Textures
//  -   -   -   -   -   -   -

struct RHITextureDesc {
    TextureDimension dimension = TextureDimension::Tex2D;
    PixelFormat format = PixelFormat::Unknown;
    UIntVec extent;
    u32 mipLevels = 1;
    u32 arrayLayers = 1;
    u32 sampleCount = 1;
    TextureUsage usage = TextureUsage::None;

    static RHITextureDesc Texture2D(PixelFormat format, UIntPoint extent, TextureUsage usage) {
        RHITextureDesc desc;
        desc.dimension = TextureDimension::Tex2D;
        desc.format = format;
        desc.extent = {extent.x, extent.y, 1};
        desc.usage = usage;
        return desc;
    }
};

class RHITexture : public RHIResource {
public:
    [[nodiscard]] RHITextureDesc const& Desc() const { return desc; }

protected:
    explicit RHITexture(RHITextureDesc const& inDesc) : desc(inDesc) {}

private:
    RHITextureDesc desc;
};

//  -   -   -   -   -   -   -
//  Buffers
//  -   -   -   -   -   -   -

struct RHIBufferDesc {
    u64 size;
    BufferUsage usage = BufferUsage::None;
};

class RHIBuffer : public RHIResource {
public:
    [[nodiscard]] RHIBufferDesc const& Desc() const { return desc; }
    [[nodiscard]] BufferUsage Usage() const { return desc.usage; }
    [[nodiscard]] u64 Size() const { return desc.size; }

    [[nodiscard]] u64 GetGpuAddress() const { return gpuAddress; }

protected:
    explicit RHIBuffer(RHIBufferDesc const& inDesc, u64 inAddress) : desc(inDesc), gpuAddress(inAddress) {}

private:
    RHIBufferDesc desc;
    u64 gpuAddress;
};

class RHIMappedBuffer : public RHIBuffer {
public:
    [[nodiscard]] void* MappedPointer() const { return mappedPointer; }

protected:
    explicit RHIMappedBuffer(RHIBufferDesc const& inDesc, u64 inAddress, void* inPtr)
        : RHIBuffer(inDesc, inAddress), mappedPointer(inPtr) {}

private:
    void* mappedPointer;
};

//  -   -   -   -   -   -   -
//  Shaders
//  -   -   -   -   -   -   -

struct RHIShaderDesc {
    Array<u32> codeBytes;
};

class RHIShader : public RHIResource {
public:
    explicit RHIShader(ShaderStage stage) : stage(stage) {}

    [[nodiscard]] ShaderStage Stage() const { return stage; }

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

//  -   -   -   -   -   -   -
//  Pipelines
//  -   -   -   -   -   -   -

struct RHIGraphicsPipelineDesc {
    RHIVertexShader* vertexShader;
    RHIPixelShader* pixelShader;

    RHIRasterState raster;
    RHIBlendState blend;

    u32 pushConstantSize;
    u32 pushConstantOffset;

    PixelFormat colorFormat;
    PixelFormat depthFormat;
};

class RHIGraphicsPipeline : public RHIResource {
public:
    [[nodiscard]] RHIGraphicsPipelineDesc const& Desc() const { return desc; }

protected:
    explicit RHIGraphicsPipeline(RHIGraphicsPipelineDesc const& inDesc) : desc(inDesc) {}

    RHIGraphicsPipelineDesc desc;
};