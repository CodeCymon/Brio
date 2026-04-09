#include "Engine/Engine.h"

#include "Platform/Platform.h"
#include "Renderer/Renderer.h"

Engine::Engine(Config const &config)
    : config_(config)
    , platform_(std::make_unique<Platform>())
    , renderer_(std::make_unique<Renderer>())
{}

Engine::~Engine() = default;

void Engine::init() {
    Platform::Config platformConfig = {
        .width = config_.width,
        .height = config_.height,
        .title = config_.title,
        .resizable = true
    };
    platform_->init(platformConfig);

    Renderer::Config renderConfig = {
        .platform = *platform_,
        .width = config_.width,
        .height = config_.height
    };
    renderer_->init(renderConfig);
}

void Engine::shutdown() {
    renderer_->shutdown();
    platform_->shutdown();
}

void Engine::run() {
    while (!platform_->shouldClose()) {
        platform_->pollEvents();
        // TODO: temporary code for handling a resize event
        u32 width, height;
        if (platform_->resizeEvent(&width, &height)) {
            renderer_->onResize(width, height);
        }
        // temporary code end
        f32 deltaTime = 0.1666666666666f;
        tick(deltaTime);
        render();
    }
}

void Engine::tick(f32 deltaTime) {
    
}

void Engine::render() {
    renderer_->render();
}
