// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "EngineCoreAPI.h"
#include "Shader.h"
#include "Window.h"

class ENGINECORE_API Engine {
public:
    Engine() = default;

    bool Initialize();

    void Run();

    void Shutdown();

private:
    [[nodiscard]] bool IsRunning() const { return bRunning; }

    void Stop() {
        bRunning = false;
    }

private:
    Window mainWindow;
    bool bRunning {false};

    GlobalShaderMap globalShaderMap;
};
