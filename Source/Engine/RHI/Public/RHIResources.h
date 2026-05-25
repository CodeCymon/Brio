#pragma once

#include "CoreMinimal.h"

struct FTextureDesc {
    u32 width {0};
    u32 height {0};
};

class FRHITexture {
public:
    FRHITexture(FTextureDesc const& inDesc) : desc(inDesc) {}
    virtual ~FRHITexture() = default;

    [[nodiscard]] FTextureDesc const& GetDesc() const { return desc; }

protected:
    FTextureDesc desc {};
};

struct FClearColor {
    union {
        struct {f32 r; f32 g; f32 b; f32 a; };
        struct {i32 r; i32 g; i32 b; i32 a; } i;
        struct {u32 r; u32 g; u32 b; u32 a; } u;
    };
};

struct FRHIFrameContext {
    FRHITexture* swapchainImage;
    u32 frameIndex;
};


