# RHI Module

add_engine_module(RHI
    PUBLIC_DEPS
        Core
    PRIVATE_DEPS
        Vulkan::Vulkan
        Platform
    SOURCES
        Private/DynamicRHI.cpp
        Internal/VulkanRHI/VulkanRHI.cpp
        Internal/VulkanRHI/Device.cpp
        Internal/VulkanRHI/Swapchain.cpp
        Internal/VulkanRHI/FrameContext.cpp
)