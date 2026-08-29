brio_add_module(
    NAME VulkanRHI

    PUBLIC_LINK
        Brio::Core

    PRIVATE_LINK
        Brio::RHI
        Brio::Platform
        VMA
        Vulkan::Vulkan
)