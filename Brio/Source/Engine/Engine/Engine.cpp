#include "Engine/Engine.h"

#include "Platform/Platform.h"
#include "RHI/VulkanDevice.h"

Engine::Engine(Config const &config)
    : config(config)
    , platform(std::make_unique<Platform>())
    , rhiDevice(std::make_unique<VulkanDevice>())
{}

Engine::~Engine() = default;

void Engine::init() {
    Platform::Config platformConfig = {
        .width = config.width,
        .height = config.height,
        .title = config.title,
        .resizable = true
    };
    platform->init(platformConfig);

    VulkanDevice::Config deviceConfig = {
        .platformSurfaceFn = [&](void* instance) -> void* {
            return platform->getSurface(instance);
        },
        .platformExtensionsFn = Platform::getExtensions,
        .validation = true
    };
    rhiDevice->init(deviceConfig);
}

void Engine::shutdown() {
    rhiDevice->shutdown();
    platform->shutdown();
}

void Engine::run() {
    while (!platform->shouldClose()) {
        platform->pollEvents();
        f32 deltaTime = 0.1666666666666f;
        tick(deltaTime);
        render();
    }
}

void Engine::tick(f32 deltaTime) {
    
}

void Engine::render() {
    
}
