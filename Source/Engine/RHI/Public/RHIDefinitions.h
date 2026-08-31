// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Core/CoreTypes.h"

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

struct Extent2D {
    u32 width = 1, height = 1;
};

struct Extent3D {
    u32 width = 1, height = 1, depth = 1;
};

enum class ShaderStage : u8 {
    Vertex,
    Pixel,
    Compute,
};