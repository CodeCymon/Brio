brio_add_module(
        NAME RHI
        SOURCES
            Private/RHI.cpp
            Private/Vulkan/VulkanRHI.cpp
            Private/Vulkan/VulkanCommandList.cpp
            Private/Vulkan/VulkanInstance.cpp
        PUBLIC_LINK
            Core
        PRIVATE_LINK
            Vulkan::Vulkan
)