#pragma once

#include "Common/Types.h"
#include "Core/Containers/Array.h"
#include "RHI/Vulkan/Device.h"
#include "RHI/Vulkan/Swapchain.h"
#include "RHI/Vulkan/FrameContext.h"

class Renderer {
public:
    struct Config {
        class Platform const& platform;
        u32 width;
        u32 height;
    };

    void init(Config const& config);
    void shutdown();

    void render();

private:


private:
    static constexpr u32 FRAMES_IN_FLIGHT = 2;
    u32 frame_index_ { 0 };

    VulkanDevice                                        device_;
    VulkanSwapchain                                     swapchain_;
    // MemoryAllocator                                  allocator_;
    // DeletionQueue                                    deleter_;
    // UploadScheduler                                  uploader_;
    // ResourceManager                                  resources_;
    // ShaderCache                                      shaders_;
    // PipelineCache                                    pipelines_;
    // DescriptorAllocator                              descriptors_;
    TStaticArray<VulkanFrameContext, FRAMES_IN_FLIGHT>  frames_;
    // RenderGraphBuilder                               graph_builder_;
};
