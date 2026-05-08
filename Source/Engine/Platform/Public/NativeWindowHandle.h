#pragma once

struct FNativeWindowHandle {
    union {
        struct { void* wldisplay; void* wlsurface; } wayland;
    };
};