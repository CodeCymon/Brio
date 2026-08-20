brio_add_module(
        NAME RHI
        SOURCES
            Private/RHI.cpp
            Private/Vulkan/VulkanRHI.cpp
            Private/Vulkan/VulkanCommandList.cpp
            Private/Vulkan/VulkanInstance.cpp
            Private/Vulkan/VulkanSurface.cpp
            Private/Vulkan/VulkanDevice.cpp
        PUBLIC_LINK
            Brio::Core
        PRIVATE_LINK
            Vulkan::Vulkan
            Brio::Platform
)