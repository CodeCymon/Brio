// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "ShaderCompilerAPI.h"

#include "CoreMinimal.h"

namespace SHADERCOMPILER_API ShaderCompiler {
    void Initialize();
    void Shutdown();

    struct CompileResult {
        Array<u32> byteCode;
        bool bSuccess;
    };

    CompileResult CompileFromFile(std::string_view path, std::string_view entryPoint);
}