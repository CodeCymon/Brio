brio_add_module(
    NAME EngineCore

    PUBLIC_LINK
        Brio::Core
        Brio::Platform

    PRIVATE_LINK
        Brio::VulkanRHI
        Brio::RHI
        Brio::ShaderCompiler
)