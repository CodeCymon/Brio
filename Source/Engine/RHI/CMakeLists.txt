# RHI Module

add_engine_module(RHI
    PUBLIC_DEPS
        Core
    PRIVATE_DEPS
        Vulkan::Vulkan
        Platform
    SOURCES
        Private/DynamicRHI.cpp
        Private/VulkanRHI/VulkanRHI.cpp
        Private/VulkanRHI/Device.cpp
        Private/VulkanRHI/Swapchain.cpp
        Private/VulkanRHI/FrameContext.cpp
)