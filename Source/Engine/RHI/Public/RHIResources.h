#pragma once

#include "CoreMinimal.h"
#include "RHIMinimal.h"

enum class EPixelFormat : u16 {
    Unknown,
    RGBA8_SRGB,
    BGRA8_SRGB,
    RGB8_SRGB,
    BGR8_SRGB,
};

struct FExtent2D {
    u32 width {0};
    u32 height {0};
};

struct FClearColor {
    union {
        struct {f32 r; f32 g; f32 b; f32 a; };
        struct {i32 r; i32 g; i32 b; i32 a; } i;
        struct {u32 r; u32 g; u32 b; u32 a; } u;
    };
};



struct FTextureDesc {
    u32 width {0};
    u32 height {0};

    EPixelFormat format {EPixelFormat::Unknown};

    FExtent2D Extent() const {
        return {width, height};
    }
};

class FRHITexture {
public:
    FRHITexture(FTextureDesc const& inDesc) : desc(inDesc) {}
    virtual ~FRHITexture() = default;

    [[nodiscard]] FTextureDesc const& GetDesc() const { return desc; }

protected:
    FTextureDesc desc {};
};



enum class EShaderStage : u8 {
    Vertex,
    Pixel,
    Compute
};

struct FShaderDesc {
    char const* path {nullptr};
    EShaderStage stage {EShaderStage::Vertex};
};

class FRHIShader {
public:
    FRHIShader(FShaderDesc const& inDesc) : desc(inDesc) {}
    virtual ~FRHIShader() = default;

    [[nodiscard]] FShaderDesc const& GetDesc() const { return desc; }

protected:
    FShaderDesc desc;
};




enum class EFillMode : u8 { Solid, Wireframe };
enum class ECullMode : u8 { None, Front, Back };
enum class EBlendMode : u8 { Opaque, Additive, AlphaBlend };

struct FRasterizerState {
    EFillMode fillMode {EFillMode::Solid};
    ECullMode cullMode {ECullMode::Back};
    bool useDepthClamp {false};
};

struct FBlendState {
    EBlendMode blendMode {EBlendMode::Opaque};
};

struct FGraphicsPipelineDesc {
    FRHIShaderRef vertexShader {nullptr};
    FRHIShaderRef pixelShader {nullptr};
    FRasterizerState rasterizer {};
    FBlendState blend {};
    EPixelFormat colorFormat {EPixelFormat::RGBA8_SRGB};
};

class FRHIGraphicsPipeline {
public:
    FRHIGraphicsPipeline(FGraphicsPipelineDesc const& inDesc) : desc(inDesc) {}
    virtual ~FRHIGraphicsPipeline() = default;

    [[nodiscard]] FGraphicsPipelineDesc const& GetDesc() const { return desc; }

protected:
    FGraphicsPipelineDesc desc;
};


struct FRHIFrameContext {
    FRHITextureRef swapchainImage;
    u32 frameIndex;
};


