# Renderer Module

add_engine_module(Renderer
    PUBLIC_DEPS
        Core
    PRIVATE_DEPS
        RHI
    SOURCES
        Private/Renderer.cpp
)

compile_shaders(Renderer
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/Shaders
    SOURCES
        ${CMAKE_SOURCE_DIR}/Shaders/Triangle.hlsl:vs:VSMain
        ${CMAKE_SOURCE_DIR}/Shaders/Triangle.hlsl:ps:PSMain
)