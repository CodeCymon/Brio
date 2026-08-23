// Copyright (c) Simon Kirsch 2026.

#pragma once
#include <atomic>

#include "CoreMinimal.h"
#include "Memory/RefCountedPtr.h"

using RHITextureRef = TRefCountedPtr<class RHITexture>;


class RHIResource {
public:
    virtual ~RHIResource() = default;
    void AddRef() { ++refCount; }
    void Release() { if (--refCount == 0) OnRefCountZero(); }

    virtual void Destroy() = 0;

protected:
    RHIResource() = default;
    virtual void OnRefCountZero() = 0;

private:
    std::atomic<u32> refCount {0};
};


enum class TextureDimension : u8 {
    Tex1D,
    Tex2D,
    Tex3D,
    Cube,
    Tex2DArray,
};

enum class TextureUsage : u32 {
    None = 0,
    Sampled = 1<<0,
    Storage = 1<<1,
    ColorAttachment = 1<<2,
    DepthStencil = 1<<3,
    TransferSrc = 1<<4,
    TransferDst = 1<<5,
};
ENABLE_ENUM_BITWISE_OPERATORS(TextureUsage);

enum class PixelFormat : u16 {
    Unknown = 0,
    RGBA8_UNORM,
    BGRA8_UNORM,
    RGBA8_SRGB,
    BGRA8_SRGB,
    RGBA16_FLOAT,
    RGBA32_FLOAT,
    D32_FLOAT,
    D24_UNORM_S8_FLOAT,
};

struct Extent2D {
    u32 width = 1, height = 1;
};

struct Extent3D {
    u32 width = 1, height = 1, depth = 1;
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