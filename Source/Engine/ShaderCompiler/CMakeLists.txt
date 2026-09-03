brio_add_module(
    NAME ShaderCompiler

    PUBLIC_LINK
        Brio::Core

    PRIVATE_LINK
        slang::slang
)

target_compile_definitions(ShaderCompiler PRIVATE DEFAULT_SHADER_DIRECTORY="${CMAKE_SOURCE_DIR}/Shaders/")