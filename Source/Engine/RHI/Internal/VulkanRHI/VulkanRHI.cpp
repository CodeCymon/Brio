#include "VulkanRHI.h"

#include <filesystem>
#include <vulkan/vk_enum_string_helper.h>

#include "RHIMinimal.h"
#include "Filesystem/FileRead.h"
#include "VulkanRHI/Utils/TranslationUtils.h"

VulkanRHI::VulkanRHI() = default;

VulkanRHI::~VulkanRHI() = default;

void VulkanRHI::Initialize(FNativeWindowHandle const &windowHandle) {
    device.Initialize({.windowHandle = windowHandle, .bValidationEnabled = true});

    swapchain.Initialize({&device, 800, 450});

    for (auto& frame: frames)
        frame.Initialize(&device);


    LOG_INFO(LogRHI, "VulkanRHI initialized.");
}

void VulkanRHI::Shutdown() {

    vkDeviceWaitIdle(device.LogicalDevice());

    for (auto& frame: frames)
        frame.Shutdown();

    swapchain.Shutdown();

    device.Shutdown();

    LOG_INFO(LogRHI, "VulkanRHI shutdown.");
}

void VulkanRHI::WaitForIdle() {
    vkDeviceWaitIdle(device.LogicalDevice());
}

void VulkanRHI::OnResize(u32 width, u32 height) {
    if (width == 0 || height == 0)
        return;

    vkDeviceWaitIdle(device.LogicalDevice());
    swapchain.Resize(width, height);
}

FRHIFrameContext VulkanRHI::BeginFrame() {
    VulkanFrameContext& frame = frames[frameIndex];

    frame.WaitForFence();

    VkResult acquired = swapchain.AcquireNextImage(frame.GetAcquireSemaphore());
    if (acquired != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "AcquireNextImage failed! {}", string_VkResult(acquired));
    }

    frame.ResetFence();

    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    vkBeginCommandBuffer(frame.Cmd(), &beginInfo);

    FVulkanTexture* swapchainTexture = swapchain.GetCurrentTexture();

    return FRHIFrameContext{
        .swapchainImage = swapchainTexture,
        .frameIndex = frameIndex
    };
}

void VulkanRHI::EndFrame() {
    VulkanFrameContext& frame = frames[frameIndex];

    vkEndCommandBuffer(frame.Cmd());

    VkCommandBuffer commands[] = { frame.Cmd() };
    VkSemaphore waitSemaphores[] = { frame.GetAcquireSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { swapchain.GetSubmitSemaphore() };
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = commands,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    vkQueueSubmit(device.GraphicsQueue(), 1, &submitInfo, frame.Fence());

    VkResult presented = swapchain.Present();
    if (presented != VK_SUCCESS) {
        LOG_WARNING(LogRHI, "Presenting swapchain failed! {}", string_VkResult(presented));
    }

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

FRHIShaderRef VulkanRHI::CreateShader(FShaderDesc const &desc) {
    std::string compiledShaderDirectory = CompiledShaderDir;
    std::string shaderExtension;
    switch (desc.stage) {
        case EShaderStage::Vertex:
            shaderExtension = ".vs.spv";
            break;
        case EShaderStage::Pixel:
            shaderExtension = ".ps.spv";
            break;
        case EShaderStage::Compute:
            shaderExtension = ".cs.spv";
            break;
    }
    std::string shaderPath = compiledShaderDirectory + "/" + std::string(desc.path) + shaderExtension;

    TArray<u8> byteCode = ReadBinaryFile(shaderPath.c_str());

    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = byteCode.Num(),
        .pCode = reinterpret_cast<u32 const*>(byteCode.Data()),
    };

    FVulkanShader* shader = shaderPool.Allocate(desc);
    vkCreateShaderModule(device.LogicalDevice(), &shaderInfo, nullptr, &shader->module);
    return shader;
}

void VulkanRHI::DestroyShader(FRHIShaderRef shader) {
    FVulkanShader* vkShader = static_cast<FVulkanShader*>(shader);
    vkDestroyShaderModule(device.LogicalDevice(), vkShader->module, nullptr);
    shaderPool.Free(vkShader);
}

FRHITextureRef VulkanRHI::CreateTexture(FTextureDesc const &desc) {
    FVulkanTexture* texture = texturePool.Allocate(desc);
    return texture;
}

void VulkanRHI::DestroyTexture(FRHITextureRef texture) {
    // TODO
}

FRHIGraphicsPipelineRef VulkanRHI::CreateGraphicsPipeline(FGraphicsPipelineDesc const &desc) {
    FVulkanGraphicsPipeline* pipeline = pipelinePool.Allocate(desc);

    VkPipelineLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    vkCreatePipelineLayout(device.LogicalDevice(), &layoutInfo, nullptr, &pipeline->layout);

    FVulkanShader* vs = static_cast<FVulkanShader*>(desc.vertexShader);
    FVulkanShader* ps = static_cast<FVulkanShader*>(desc.pixelShader);

    VkPipelineShaderStageCreateInfo stages[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vs->module,
            .pName = "VSMain",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = ps->module,
            .pName = "PSMain"
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInput {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {
        .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates    = dynamicStates
    };

    VkPipelineViewportStateCreateInfo viewportState {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount  = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer {
        .sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = desc.rasterizer.fillMode == EFillMode::Solid
                        ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE,
        .cullMode    = TranslationUtils::CullMode(desc.rasterizer.cullMode),
        .lineWidth   = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisample {
        .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkPipelineColorBlendAttachmentState blendAttachment {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo blendState {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments    = &blendAttachment,
    };

    VkFormat colorFormat = TranslationUtils::PixelFormat(desc.colorFormat);
    VkPipelineRenderingCreateInfo renderingInfo {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &colorFormat,
    };

    VkGraphicsPipelineCreateInfo pipelineInfo {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &renderingInfo,
        .stageCount          = 2,
        .pStages             = stages,
        .pVertexInputState   = &vertexInput,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisample,
        .pColorBlendState    = &blendState,
        .pDynamicState       = &dynamicState,
        .layout              = pipeline->layout,
    };

    vkCreateGraphicsPipelines(device.LogicalDevice(), VK_NULL_HANDLE,
        1, &pipelineInfo, nullptr, &pipeline->pipeline);

    return pipeline;
}

void VulkanRHI::DestroyGraphicsPipeline(FRHIGraphicsPipelineRef graphicsPipeline) {
    FVulkanGraphicsPipeline* vkGraphicsPipeline = static_cast<FVulkanGraphicsPipeline*>(graphicsPipeline);
    vkDestroyPipelineLayout(device.LogicalDevice(), vkGraphicsPipeline->layout, nullptr);
    vkDestroyPipeline(device.LogicalDevice(), vkGraphicsPipeline->pipeline, nullptr);
    pipelinePool.Free(vkGraphicsPipeline);
}

void VulkanRHI::BeginRendering(FRHITextureRef colorTarget) {
    FVulkanTexture* vkTexture = static_cast<FVulkanTexture*>(colorTarget);
    VulkanFrameContext& frame = frames[frameIndex];

    {
        // TODO: remove
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .dstQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .image = vkTexture->image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1,
            }
        };

        VkDependencyInfo dependencyInfo = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &imageBarrier
        };
        vkCmdPipelineBarrier2(frame.Cmd(), &dependencyInfo);
    }

    VkRenderingAttachmentInfo colorAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = vkTexture->view,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {.color = {0.1,0.1,0.1, 1.0}}
    };

    VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {{0,0}, swapchain.Extent()},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
    };

    vkCmdBeginRendering(frame.Cmd(), &renderingInfo);
}

void VulkanRHI::EndRendering(FRHITextureRef colorTarget) {
    FVulkanTexture* vkTexture = static_cast<FVulkanTexture*>(colorTarget);
    VulkanFrameContext& frame = frames[frameIndex];

    vkCmdEndRendering(frame.Cmd());

    {
        // TODO: remove
        VkImageMemoryBarrier2 imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .dstQueueFamilyIndex = device.GraphicsFamilyIndex(),
            .image = vkTexture->image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1,
            }
        };

        VkDependencyInfo dependencyInfo = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &imageBarrier
        };
        vkCmdPipelineBarrier2(frame.Cmd(), &dependencyInfo);
    }

}

void VulkanRHI::BindPipeline(FRHIGraphicsPipelineRef graphicsPipeline) {
    FVulkanGraphicsPipeline* vkGraphicsPipeline = static_cast<FVulkanGraphicsPipeline*>(graphicsPipeline);
    VulkanFrameContext& frame = frames[frameIndex];

    vkCmdBindPipeline(frame.Cmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline->pipeline);
}

void VulkanRHI::SetViewport(f32 x, f32 y, f32 width, f32 height) {
    VulkanFrameContext& frame = frames[frameIndex];

    VkViewport viewport = {
        x, y,
        width, height,
        1, 0
    };
    vkCmdSetViewport(frame.Cmd(), 0, 1, &viewport);
}

void VulkanRHI::SetScissor(i32 x, i32 y, u32 width, u32 height) {
    VulkanFrameContext& frame = frames[frameIndex];

    VkRect2D scissor = {
        {x, y},
        {width, height}
    };
    vkCmdSetScissor(frame.Cmd(), 0, 1, &scissor);
}

void VulkanRHI::Draw(u32 numVertices, u32 firstVertex) {
    VulkanFrameContext& frame = frames[frameIndex];
    vkCmdDraw(frame.Cmd(), numVertices, 1, firstVertex, 0);
}
