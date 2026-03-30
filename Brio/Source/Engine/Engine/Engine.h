#pragma once

#include <Common/Defines.h>
#include <memory>

class Engine {
public:
    struct Config {
        u32 width { 1280 };
        u32 height { 720 };
        const char* title { "Engine" };
    };

    BAPI explicit Engine(Config const &config);
    BAPI ~Engine();

    BAPI void init();
    BAPI void shutdown();

    BAPI void run();

private:
    void tick(f32 deltaTime);
    void render();

private:
    Config const &config;

    std::unique_ptr<class Platform> platform;
    std::unique_ptr<class VulkanDevice> rhiDevice;
};
