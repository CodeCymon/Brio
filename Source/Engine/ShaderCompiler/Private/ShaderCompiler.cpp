// Copyright (c) Simon Kirsch 2026.

#include "ShaderCompiler.h"

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

DEFINE_LOG_CATEGORY_STATIC(LogShaderCompiler);

namespace {
    Slang::ComPtr<slang::IGlobalSession> gGlobalSession;
    Slang::ComPtr<slang::ISession> gSession;
}

bool ShaderCompiler::Initialize() {
    slang::createGlobalSession(gGlobalSession.writeRef());

    slang::TargetDesc targetDesc = {};
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = gGlobalSession->findProfile("spirv_1_5");

    const char* searchPaths[] = {
        DEFAULT_SHADER_DIRECTORY
    };

    slang::SessionDesc sessionDesc = {};
    sessionDesc.targetCount = 1;
    sessionDesc.targets = &targetDesc;
    sessionDesc.searchPathCount = 1;
    sessionDesc.searchPaths = searchPaths;

    gGlobalSession->createSession(sessionDesc, gSession.writeRef());
    return true;
}

void ShaderCompiler::Shutdown() {
    gSession = nullptr;
    gGlobalSession = nullptr;
}

ShaderCompiler::CompileResult ShaderCompiler::CompileFromFile(std::string_view path,
    std::string_view entryPoint) {

    Slang::ComPtr<slang::IBlob> diagnosticsBlob;

    Slang::ComPtr<slang::IModule> module;
    module = gSession->loadModule(path.data(), diagnosticsBlob.writeRef());

    if (diagnosticsBlob) {
        LOG_ERROR(LogShaderCompiler, "{}", (const char*)diagnosticsBlob->getBufferPointer());
    }

    Slang::ComPtr<slang::IEntryPoint> slangEntryPoint;
    module->findEntryPointByName(entryPoint.data(), slangEntryPoint.writeRef());
    if (!slangEntryPoint) {
        LOG_ERROR(LogShaderCompiler, "Entry point: {} not found", entryPoint.data());
    }

    StaticArray<slang::IComponentType*, 2> componentTypes = { module, slangEntryPoint };
    Slang::ComPtr<slang::IComponentType> composedProgram;
    SlangResult result = gSession->createCompositeComponentType(
        componentTypes.Data(), componentTypes.Size(),
        composedProgram.writeRef(), diagnosticsBlob.writeRef()
    );
    if (SLANG_FAILED(result)) {
        LOG_ERROR(LogShaderCompiler, "{}", (const char*)diagnosticsBlob->getBufferPointer());
        return {.bSuccess = false};
    }

    Slang::ComPtr<slang::IComponentType> linkedProgram;
    result = composedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
    if (SLANG_FAILED(result)) {
        LOG_ERROR(LogShaderCompiler, "{}", (const char*)diagnosticsBlob->getBufferPointer());
        return {.bSuccess = false};
    }

    Slang::ComPtr<slang::IBlob> spirvCode;
    result = linkedProgram->getEntryPointCode(0, 0, spirvCode.writeRef(), diagnosticsBlob.writeRef());
    if (SLANG_FAILED(result)) {
        LOG_ERROR(LogShaderCompiler, "{}", (const char*)diagnosticsBlob->getBufferPointer());
        return {.bSuccess = false};
    }

    const void* data = spirvCode->getBufferPointer();
    const size_t byteSize = spirvCode->getBufferSize();

    ASSERT(byteSize % sizeof(u32) == 0);

    const auto* spirvData = static_cast<const u32*>(data);
    u32 wordCount = byteSize / sizeof(u32);

    LOG_INFO(LogShaderCompiler, "Compiled '{}::{}' with {} bytes ({} SPIR-V words)", path, entryPoint, byteSize, wordCount);

    return {
        .byteCode = Array{spirvData, wordCount},
        .bSuccess = true
    };
}