#pragma once
#include "Common/Defines.h"
#include "Core/Containers/Array.h"

class Platform {
public:
    struct Config {
        u32 width{1280};
        u32 height{720};
        const char *title{"Engine"};
        bool resizable{true};
    };

    Platform(Platform const &) = delete;
    Platform &operator=(Platform const &) = delete;

    Platform();
    ~Platform();

    void init(Config const &config);
    void shutdown();

    void pollEvents();
    [[nodiscard]] bool shouldClose() const;

    void *getSurface(void *instance);
    static TArray<const char*> getExtensions();

private:
    struct Impl;
    Impl *data{nullptr};
};
