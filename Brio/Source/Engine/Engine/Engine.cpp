#include "Engine/Engine.h"

#include "Platform/Platform.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanSwapchain.h"

Engine::Engine(Config const &config)
    : config(config)
    , platform(std::make_unique<Platform>())
    , rhiDevice(std::make_unique<VulkanDevice>())
    , swapchain(std::make_unique<VulkanSwapchain>())
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

    VulkanSwapchain::Config swapchainConfig = {
        .device = rhiDevice.get(),
        .width = config.width,
        .height = config.height
    };
    swapchain->init(swapchainConfig);
}

void Engine::shutdown() {
    swapchain->shutdown();
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
