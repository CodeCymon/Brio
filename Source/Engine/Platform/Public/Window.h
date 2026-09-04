// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "PlatformAPI.h"
#include "Core/CoreTypes.h"
#include "Delegates/SinglecastDelegate.h"
#include "Math/IntPoint.h"

struct NativeWindowData {
    union {
        struct { void* wldisplay; void* wlsurface; } wayland{};
    };
};

class PLATFORM_API Window {
public:
    struct Config {
        u32 width{1280};
        u32 height{720};
        char const* title{"Brio"};
        bool resizable{true};
    };

    static bool Create(Config const& config, Window& window);
    void Close();

public:
    [[nodiscard]] NativeWindowData NativeData() const;
    [[nodiscard]] UIntPoint Extent() const { return extent; }

public:
    Delegate<void()> OnCloseDelegate;
    // TODO: make OnResizeDelegate multicast
    Delegate<void(u32, u32)> OnResizeDelegate;

private:
    struct GLFWwindow* handle{nullptr};
    UIntPoint extent;
};
