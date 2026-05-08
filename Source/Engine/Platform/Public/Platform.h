#pragma once

#include "PlatformAPI.h"

#include "Types/CoreTypes.h"
#include "NativeWindowHandle.h"
#include "Delegates/MulticastDelegate.h"

class PLATFORM_API Platform {
public:
    struct FConfig {
        u32 width {1280};
        u32 height {720};
        char const* title {"Engine"};
        bool resizable {true};
    };

    Platform(Platform const&)            = delete;
    Platform& operator=(Platform const&) = delete;

    Platform() = default;
    ~Platform() = default;

    bool Initialize(FConfig const& Config);
    void Shutdown();

    void PollEvents();

    [[nodiscard]] FNativeWindowHandle NativeHandle() const;

public:
    TMulticastDelegate<void()>          OnCloseDelegate;
    TMulticastDelegate<void(u32, u32)>  OnResizeDelegate;

private:
    struct GLFWwindow *window{nullptr};
};
