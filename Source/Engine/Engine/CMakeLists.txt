# Engine Module

add_engine_module(Engine
    PUBLIC_DEPS
        Core
    PRIVATE_DEPS
        Platform
        RHI
        Renderer
    SOURCES
        Private/Engine.cpp
        Private/TimeManager.cpp
)