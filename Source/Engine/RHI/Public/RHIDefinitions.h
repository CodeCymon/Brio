// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"
#include "Core/CoreMacros.h"

struct ClearColor {
    f32 r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

enum class TextureDimension : u8 {
    Tex1D,
    Tex2D,
    Tex3D,
    // Cube,
    // Tex2DArray,
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
    D24_UNORM_S8_UINT,
};


enum class ShaderStage : u8 {
    None = 0,
    Vertex = 1<<0,
    Pixel = 1<<1,
    Compute = 1<<2,
};
ENABLE_ENUM_BITWISE_OPERATORS(ShaderStage);

enum class RHIResourceState : u8 {
    Undefined,
    TransferSrc,
    TransferDst,
    ColorAttachment,
    DepthStencilAttachment,
    DepthStencilReadOnly,
    ShaderReadOnly,
    General,
    Present,
};


enum class FillMode : u8 {
    Solid,
    Wireframe,
};

enum class CullMode : u8 {
    None,
    Front,
    Back,
};

enum class BlendMode : u8 {
    Opaque,
    Additive,
    AlphaBlend,
};

struct RHIRasterState {
    FillMode fillMode;
    CullMode cullMode;
    bool bClampDepth;
};

struct RHIBlendState {
    BlendMode blendMode;
};