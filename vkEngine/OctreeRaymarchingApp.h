#pragma once

#define VK_EXT_debug_utils
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "VkClassesIndex.h"
#include "BitmaskOctree.h"

const bool enableValidationLayers = true;

const std::vector<Vertex> quadVertices = {
        {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
};

struct ChunkInfo {
    int x;
    int y;
    int z;
    int index;
};

std::vector<uint32_t> quadIndices = {
    0,1,2,
    2,3,0
};

class OctreeRaymarchingApp {

public:
    void run() {
        initSettings();
        createOctree();
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    std::shared_ptr<VkConfig> config;

    GLFWwindow* window = nullptr;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    std::shared_ptr<Instance> p_Instance;
    std::shared_ptr<Surface> p_Surface;
    std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
    std::shared_ptr<LogicalDevice> p_LogicalDevice;

    std::shared_ptr<SwapChain> p_SwapChain;
    std::shared_ptr<Images> p_Images;
    std::shared_ptr<ImageViews> p_ImageViews;
    std::shared_ptr<RenderPass> p_RenderPass;
    uint32_t colorPassBinding;
    uint32_t depthPassBinding;
    uint32_t colorResolveBinding;

    std::shared_ptr<DescriptorSetLayout> p_GraphicsDescriptorSetLayout;
    std::shared_ptr<DescriptorSetLayout> p_DepthComputeDescriptorSetLayout;
    std::shared_ptr<DescriptorSetLayout> p_ComputeDescriptorSetLayout;
    uint32_t vertexUniformBinding;
    uint32_t fragmentSamplerBinding;
    uint32_t imguiSamplerBinding;

    uint32_t depthComputeUniformBinding;
    uint32_t depthComputeSSBOBinding;
    uint32_t depthComputeChunkInfoSSBOBinding;
    uint32_t depthComputeDepthImageBinding;
    uint32_t depthComputeImageBinding;

    uint32_t computeUniformBinding;
    uint32_t computeSSBOBinding;
    uint32_t computeChunkInfoSSBOBinding;
    uint32_t computeDepthImageBinding;
    uint32_t computeImageBinding;

    std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
    std::shared_ptr<ComputePipeline> p_DepthComputePipeline;
    std::shared_ptr<ComputePipeline> p_ComputePipeline;

    std::shared_ptr<CommandPool> p_CommandPool;

    std::shared_ptr<StorageImage> p_DepthImage;
    std::shared_ptr<SSBO> p_OctreeSSBO;
    std::shared_ptr<SSBO> p_ChunkInfoSSBO;
    std::shared_ptr<StorageImage> p_CanvasImage;
    std::shared_ptr<Sampler> p_CanvasSampler;
    std::shared_ptr<UniformBuffer> p_ComputeUBO;
    std::shared_ptr<UniformBuffer> p_VertexUBO;
    std::shared_ptr<VertexBuffer> p_VertexBuffer;
    std::shared_ptr<IndexBuffer> p_IndexBuffer;

    std::shared_ptr<DescriptorPool> p_DepthComputeDescriptorPool;
    std::shared_ptr<DescriptorSets> p_DepthComputeDescriptorSets;
    std::shared_ptr<DescriptorPool> p_ComputeDescriptorPool;
    std::shared_ptr<DescriptorSets> p_ComputeDescriptorSets;
    std::shared_ptr<DescriptorPool> p_GraphicsDescriptorPool;
    std::shared_ptr<DescriptorSets> p_GraphicsDescriptorSets;

    std::shared_ptr<DepthResources> p_DepthResources;
    std::shared_ptr<ColorResources> p_ColorResources;

    std::shared_ptr<FrameBuffers> p_FrameBuffers;

    std::shared_ptr<Gui> p_Gui;

    std::shared_ptr<CommandBuffers> p_CommandBuffers;
    std::shared_ptr<ComputeCommandBuffers> p_ComputeCommandBuffers;
    std::shared_ptr<ComputeCommandBuffers> p_DepthComputeCommandBuffers;

    std::shared_ptr<SyncObjects> p_SyncObjects;

    QueueSubmitter queuesubmit;

    std::vector<unsigned int> octreeArray;
    std::vector<ChunkInfo> chunkInfos;

    void initSettings() {
        config = std::make_shared<VkConfig>();
        config->enableValidationLayers = enableValidationLayers;
        config->deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME };
        config->validationLayers = { "VK_LAYER_KHRONOS_validation"};
        config->computeEnabled = true;
        config->msaaEnabled = false;
        config->depthStencilEnabled = false;
        config->downScaleFactor = 4;
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(config->screenWidth, config->screenHeight, "VkEngine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<OctreeRaymarchingApp*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        initDevices();
        initPresentation();
        initDescriptorSetLayouts();
        initPipelines();
        initCommandPool();
        initShaderResources();
        initDescriptorSets();
        //initRenderAttachments();
        initFrameBuffers();
        initCommandBuffers();
        initSyncObjects();
    }

    void initDevices() {
        p_Instance = std::make_shared<Instance>(config);
        p_Instance->create();

        p_Surface = std::make_shared<Surface>(
            p_Instance->instance,
            window
        );

        p_PhysicalDevice = std::make_shared<PhysicalDevice>(
            p_Instance->instance,
            p_Surface->surface,
            config
        );
        p_PhysicalDevice->create();

        p_LogicalDevice = std::make_shared<LogicalDevice>(
            p_PhysicalDevice,
            config
        );
        p_LogicalDevice->create();
    }

    void initPresentation() {
        p_SwapChain = std::make_shared<SwapChain>(
            window,
            p_Surface->surface,
            p_LogicalDevice->device,
            p_PhysicalDevice->queueFamilies,
            p_PhysicalDevice->getSwapChainSupportDetails()
        );
        p_SwapChain->create();

        p_Images = std::make_shared<Images>(
            p_PhysicalDevice,
            p_LogicalDevice->device
        );

        p_ImageViews = std::make_shared<ImageViews>(p_LogicalDevice->device);
        p_ImageViews->createSwapChainImageViews(
            p_SwapChain->swapChainImages,
            p_SwapChain->swapChainImageFormat
        );

        p_RenderPass = std::make_shared<RenderPass>(
            p_PhysicalDevice->physicalDevice,
            p_PhysicalDevice->msaaSamples,
            p_LogicalDevice->device,
            p_SwapChain->swapChainImageFormat,
            config
        );
        colorPassBinding = p_RenderPass->bindColorPass();
        //depthPassBinding = p_RenderPass->bindDepthPass();
        //colorResolveBinding = p_RenderPass->bindMsaaPass();
        p_RenderPass->create();
    }

    void initDescriptorSetLayouts() {
        /*
            TODO:
                1. Create parent "DescriptorSetLayout" class.
                2. Allow for similar user control in graphics DSL as what is now in compute DSL.
        */
        p_GraphicsDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(
            p_LogicalDevice->device
        );
        //vertexUniformBinding = p_GraphicsDescriptorSetLayout->bindVertUniformBuffer();  //0
        fragmentSamplerBinding = p_GraphicsDescriptorSetLayout->bindFragSampler();      //1
        imguiSamplerBinding = p_GraphicsDescriptorSetLayout->bindFragSampler();
        p_GraphicsDescriptorSetLayout->create();

        p_DepthComputeDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(p_LogicalDevice->device);
        depthComputeUniformBinding = p_DepthComputeDescriptorSetLayout->bindCompUniformBuffer();  //0
        depthComputeSSBOBinding = p_DepthComputeDescriptorSetLayout->bindCompStorageBuffer();     //1
        depthComputeChunkInfoSSBOBinding = p_DepthComputeDescriptorSetLayout->bindCompStorageBuffer();     //2
        depthComputeDepthImageBinding = p_DepthComputeDescriptorSetLayout->bindCompStorageImage();     //3
        depthComputeImageBinding = p_DepthComputeDescriptorSetLayout->bindCompStorageImage();     //4
        p_DepthComputeDescriptorSetLayout->create();

        p_ComputeDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(p_LogicalDevice->device);

        // Note: Must do DSL bindings in order of bindings in shader code
        // Should be fine to do bindings in any order for rest of code (Descriptor Pools and Sets)
        // since binding functions will be called with the binding indices returned here
        computeUniformBinding = p_ComputeDescriptorSetLayout->bindCompUniformBuffer();  //0
        computeSSBOBinding = p_ComputeDescriptorSetLayout->bindCompStorageBuffer();     //1
        computeChunkInfoSSBOBinding = p_ComputeDescriptorSetLayout->bindCompStorageBuffer();     //2
        computeDepthImageBinding = p_ComputeDescriptorSetLayout->bindCompStorageImage();    //3
        computeImageBinding = p_ComputeDescriptorSetLayout->bindCompStorageImage();     //4
        p_ComputeDescriptorSetLayout->create();
    }

    void initPipelines() {
        /*
            TODO: 
                1. Create parent "Pipeline" class with createShaderModule().
                2. Restructure pipeline creation for user control in future.
                3. Support reloading of pipelines in background threads. (for shader hot reloading)
        */
        p_GraphicsPipeline = std::make_shared<GraphicsPipeline>(
            p_PhysicalDevice->msaaSamples,
            p_LogicalDevice->device,
            p_SwapChain->swapChainExtent,
            p_RenderPass->renderPass,
            p_GraphicsDescriptorSetLayout->descriptorSetLayout,
            config
        );
        std::string vertexShaderFile = "shaders/simple_vert.spv";
        std::string fragmentShaderFile = "shaders/simple_frag.spv";
        p_GraphicsPipeline->create(
            vertexShaderFile,
            fragmentShaderFile
        );

        p_DepthComputePipeline = std::make_shared<ComputePipeline>(
            p_LogicalDevice->device,
            p_DepthComputeDescriptorSetLayout->descriptorSetLayout
        );
        std::string depthComputeShaderFile = "shaders/depthPass.spv";
        p_DepthComputePipeline->create(depthComputeShaderFile);

        p_ComputePipeline = std::make_shared<ComputePipeline>(
            p_LogicalDevice->device,
            p_ComputeDescriptorSetLayout->descriptorSetLayout
        );
        std::string computeShaderFile = "shaders/bitmask.spv";
        p_ComputePipeline->create(computeShaderFile);
    }

    void initCommandPool() {
        p_CommandPool = std::make_shared<CommandPool>(
            p_PhysicalDevice->queueFamilies,
            p_LogicalDevice->device,
            p_LogicalDevice->graphicsQueue
        );
        p_CommandPool->create();
    }

    void initShaderResources() {
        p_DepthImage = std::make_shared<StorageImage>(
            p_LogicalDevice->device,
            p_Images,
            p_ImageViews,
            p_CommandPool,
            config
        );
        p_DepthImage->imageFormat = VK_FORMAT_R32_SFLOAT;
        p_DepthImage->create(p_SwapChain->swapChainExtent);

        p_OctreeSSBO = std::make_shared<SSBO>(
            p_LogicalDevice,
            p_CommandPool,
            config
        );
        p_OctreeSSBO->uploadData(
            sizeof(unsigned int) * octreeArray.size(),
            octreeArray.data()
        );

        p_ChunkInfoSSBO = std::make_shared<SSBO>(
            p_LogicalDevice,
            p_CommandPool,
            config
        );
        p_ChunkInfoSSBO->uploadData(
            sizeof(ChunkInfo) * config->ubo.numChunks,
            chunkInfos.data()
        );

        p_CanvasImage = std::make_shared<StorageImage>(
            p_LogicalDevice->device,
            p_Images,
            p_ImageViews,
            p_CommandPool,
            config
        );
        p_CanvasImage->create(p_SwapChain->swapChainExtent);

        p_CanvasSampler = std::make_shared<Sampler>(
            p_PhysicalDevice->physicalDevice,
            p_LogicalDevice->device,
            config
        );
        p_CanvasSampler->create();

        p_ComputeUBO = std::make_shared<UniformBuffer>(
            p_LogicalDevice,
            config
        );
        p_ComputeUBO->create(2*sizeof(ComputeUniformBufferObject));

        /*
        p_VertexUBO = std::make_shared<UniformBuffer>(
            p_LogicalDevice,
            config
        );
        p_VertexUBO->create(sizeof(VertexUniformBufferObject));
        */
        p_VertexBuffer = std::make_shared<VertexBuffer>(
            p_LogicalDevice,
            p_CommandPool
        );
        p_VertexBuffer->create(quadVertices);

        p_IndexBuffer = std::make_shared<IndexBuffer>(
            p_LogicalDevice,
            p_CommandPool
        );
        p_IndexBuffer->create(quadIndices);
    }

    void initDescriptorSets() {
        p_DepthComputeDescriptorPool = std::make_shared<DescriptorPool>(
            p_LogicalDevice->device,
            config
        );
        p_DepthComputeDescriptorPool->bindUniformBuffer(computeUniformBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageBuffer(computeSSBOBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageBuffer(computeChunkInfoSSBOBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageImage(computeDepthImageBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageImage(computeImageBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->create();

        p_DepthComputeDescriptorSets = std::make_shared<DescriptorSets>(
            p_LogicalDevice->device,
            p_DepthComputeDescriptorSetLayout,
            p_DepthComputeDescriptorPool,
            config
        );
        p_DepthComputeDescriptorSets->bindUniformBuffer(
            depthComputeUniformBinding,
            p_ComputeUBO->uniformBuffers,
            2 * sizeof(ComputeUniformBufferObject)
        );
        p_DepthComputeDescriptorSets->bindStorageBuffer(
            depthComputeSSBOBinding,
            p_OctreeSSBO->shaderStorageBuffers,
            sizeof(unsigned int) * octreeArray.size()
        );
        p_DepthComputeDescriptorSets->bindStorageBuffer(
            depthComputeChunkInfoSSBOBinding,
            p_ChunkInfoSSBO->shaderStorageBuffers,
            sizeof(ChunkInfo) * config->ubo.numChunks
        );
        p_DepthComputeDescriptorSets->bindStorageImage(
            computeDepthImageBinding,
            p_DepthImage->storageImageViews
        );
        p_DepthComputeDescriptorSets->bindStorageImage(
            depthComputeImageBinding,
            p_CanvasImage->storageImageViews
        );
        p_DepthComputeDescriptorSets->create();


        p_ComputeDescriptorPool = std::make_shared<DescriptorPool>(
            p_LogicalDevice->device,
            config
        );
        p_ComputeDescriptorPool->bindUniformBuffer(computeUniformBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageBuffer(computeSSBOBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageBuffer(computeChunkInfoSSBOBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageImage(computeDepthImageBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageImage(computeImageBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->create();

        p_ComputeDescriptorSets = std::make_shared<DescriptorSets>(
            p_LogicalDevice->device,
            p_ComputeDescriptorSetLayout,
            p_ComputeDescriptorPool,
            config
        );
        p_ComputeDescriptorSets->bindUniformBuffer(
            computeUniformBinding,
            p_ComputeUBO->uniformBuffers,
            2*sizeof(ComputeUniformBufferObject)
        );
        p_ComputeDescriptorSets->bindStorageBuffer(
            computeSSBOBinding,
            p_OctreeSSBO->shaderStorageBuffers,
            sizeof(unsigned int) * octreeArray.size()
        );
        p_ComputeDescriptorSets->bindStorageBuffer(
            computeChunkInfoSSBOBinding,
            p_ChunkInfoSSBO->shaderStorageBuffers,
            sizeof(ChunkInfo) * config->ubo.numChunks
        );
        p_ComputeDescriptorSets->bindStorageImage(
            computeDepthImageBinding,
            p_DepthImage->storageImageViews
        );
        p_ComputeDescriptorSets->bindStorageImage(
            computeImageBinding,
            p_CanvasImage->storageImageViews
        );
        p_ComputeDescriptorSets->create();

        p_GraphicsDescriptorPool = std::make_shared<DescriptorPool>(
            p_LogicalDevice->device,
            config
        );
        //p_GraphicsDescriptorPool->bindUniformBuffer(vertexUniformBinding, config->maxFramesInFlight);
        p_GraphicsDescriptorPool->bindSampler(fragmentSamplerBinding, config->maxFramesInFlight);
        p_GraphicsDescriptorPool->bindSampler(imguiSamplerBinding, config->maxFramesInFlight);
        p_GraphicsDescriptorPool->create();

        p_GraphicsDescriptorSets = std::make_shared<DescriptorSets>(
            p_LogicalDevice->device,
            p_GraphicsDescriptorSetLayout,
            p_GraphicsDescriptorPool,
            config
        );
        /*
        p_GraphicsDescriptorSets->bindUniformBuffer(
            vertexUniformBinding,
            p_VertexUBO->uniformBuffers,
            sizeof(VertexUniformBufferObject)
        );
        */
        p_GraphicsDescriptorSets->bindSampler(
            fragmentSamplerBinding,
            p_CanvasImage->storageImageViews,
            p_CanvasSampler->imageSamplers
        );
        p_GraphicsDescriptorSets->create();
    }

    void initRenderAttachments() {
        p_DepthResources = std::make_shared<DepthResources>(
            p_LogicalDevice->device,
            p_Images,
            p_ImageViews,
            p_SwapChain->swapChainExtent,
            p_RenderPass->findDepthFormat(),
            p_PhysicalDevice->msaaSamples
        );
        p_DepthResources->create();

        p_ColorResources = std::make_shared<ColorResources>(
            p_LogicalDevice->device,
            p_Images,
            p_ImageViews,
            p_SwapChain->swapChainExtent,
            p_SwapChain->swapChainImageFormat,
            p_PhysicalDevice->msaaSamples
        );
        p_ColorResources->create();
    }
     
    void initFrameBuffers() {
        p_FrameBuffers = std::make_shared<FrameBuffers>(
            p_LogicalDevice->device,
            p_SwapChain,
            p_ImageViews,
            p_RenderPass->renderPass,
            config
        );
        //p_FrameBuffers->bindDepthResources(depthPassBinding, p_DepthResources->depthImageView);
        //p_FrameBuffers->bindColorReources(colorResolveBinding, p_ColorResources->colorImageView);
        //p_FrameBuffers->bindSwapChainImageViews(colorPassBinding);
        p_FrameBuffers->create(NULL, NULL);
    }

    void initCommandBuffers() {
        p_Gui = std::make_shared<Gui>(
            p_Surface->window,
            p_Instance->instance,
            p_Instance->allocator,
            p_PhysicalDevice->physicalDevice,
            p_PhysicalDevice->msaaSamples,
            p_PhysicalDevice->graphicsAndComputeFamily,
            p_LogicalDevice->device,
            p_LogicalDevice->graphicsQueue,
            p_GraphicsDescriptorPool->descriptorPool,
            p_RenderPass->renderPass,
            config
        );
        p_Gui->init();

        p_CommandBuffers = std::make_shared<CommandBuffers>(
            p_LogicalDevice->device,
            p_CommandPool->commandPool,
            config
        );
        p_CommandBuffers->attachAllResources(
            p_RenderPass->renderPass,
            p_GraphicsPipeline->graphicsPipeline,
            p_GraphicsPipeline->pipelineLayout,
            p_FrameBuffers->frameBuffers,
            p_GraphicsDescriptorSets->descriptorSets,
            p_VertexBuffer->vertexBuffer,
            p_IndexBuffer->indexBuffer
        );
        p_CommandBuffers->setExtent(p_SwapChain->swapChainExtent); 
        p_CommandBuffers->attachGui(p_Gui);
        p_CommandBuffers->create();

        p_DepthComputeCommandBuffers = std::make_shared<ComputeCommandBuffers>(
            p_LogicalDevice->device,
            p_CommandPool->commandPool,
            config
        );
        p_DepthComputeCommandBuffers->attachComputePipeline(p_DepthComputePipeline->computePipeline, p_DepthComputePipeline->computePipelineLayout);
        p_DepthComputeCommandBuffers->attachDescriptorSets(p_DepthComputeDescriptorSets->descriptorSets);

        p_DepthComputeCommandBuffers->setExtent(p_SwapChain->swapChainExtent.width / 2, p_SwapChain->swapChainExtent.height / 2);
        p_DepthComputeCommandBuffers->create();

        p_ComputeCommandBuffers = std::make_shared<ComputeCommandBuffers>(
            p_LogicalDevice->device,
            p_CommandPool->commandPool,
            config
        );
        p_ComputeCommandBuffers->attachComputePipeline(p_ComputePipeline->computePipeline, p_ComputePipeline->computePipelineLayout);
        p_ComputeCommandBuffers->attachDescriptorSets(p_ComputeDescriptorSets->descriptorSets);
        
        p_ComputeCommandBuffers->setGroupSize(
             (p_SwapChain->swapChainExtent.width * p_SwapChain->swapChainExtent.height) - 
             ((p_SwapChain->swapChainExtent.width / 2) * (p_SwapChain->swapChainExtent.height / 2))
        );
        
        /*
        p_ComputeCommandBuffers->setExtent(
            p_SwapChain->swapChainExtent.width,
            p_SwapChain->swapChainExtent.height
        );
        */
        p_ComputeCommandBuffers->create();
    }

    void initSyncObjects() {
        p_SyncObjects = std::make_shared<SyncObjects>(
            p_LogicalDevice->device,
            config
        );
        p_SyncObjects->create();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
            double currentTime = glfwGetTime();
            config->lastFrameTime = (currentTime - config->lastTime) * 1000.0;
            config->lastTime = currentTime;
        }
        p_LogicalDevice->waitIdle();
    }

    void drawFrame() {
        updateComputeUniformBuffer(currentFrame);


        // Depth Compute submission
                p_SyncObjects->waitForDepthComputeFence(currentFrame, VK_TRUE, UINT64_MAX);


                p_DepthComputeCommandBuffers->recordBuffer(
                    p_DepthComputeCommandBuffers->commandBuffers[currentFrame],
                    currentFrame
                );

                queuesubmit.submit(
                    p_LogicalDevice->computeQueue,
                    p_SyncObjects->depthComputeInFlightFences[currentFrame],
                    &p_DepthComputeCommandBuffers->commandBuffers[currentFrame],
                    &p_SyncObjects->depthComputeFinishedSemaphores[currentFrame]
                );
            
                    
        // Compute submission  
                
                {
                    p_SyncObjects->waitForComputeFence(currentFrame, VK_TRUE, UINT64_MAX);

                    p_ComputeCommandBuffers->recordBuffer(
                        p_ComputeCommandBuffers->commandBuffers[currentFrame],
                        currentFrame
                    );

                    VkSemaphore waitSemaphores[] = {
                        p_SyncObjects->depthComputeFinishedSemaphores[currentFrame],
                    };
                    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
                    queuesubmit.submit(
                        p_LogicalDevice->computeQueue,
                        p_SyncObjects->computeInFlightFences[currentFrame],
                        &p_ComputeCommandBuffers->commandBuffers[currentFrame],
                        &p_SyncObjects->computeFinishedSemaphores[currentFrame],
                        1,
                        waitSemaphores,
                        waitStages
                    );
                }
                

        // Graphics submission
                p_SyncObjects->waitForGraphicsFence(currentFrame, VK_TRUE, UINT64_MAX);

                uint32_t imageIndex;
                VkResult result = vkAcquireNextImageKHR(
                    p_LogicalDevice->device, 
                    p_SwapChain->swapChain, 
                    UINT64_MAX, 
                    p_SyncObjects->imageAvailableSemaphores[currentFrame], 
                    VK_NULL_HANDLE, 
                    &imageIndex
                );

                if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                    recreateSwapChain();
                    return;
                }
                else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                    throw std::runtime_error("failed to acquire swap chain image!");
                }
        
                p_CommandBuffers->recordBufferIndexed(currentFrame, imageIndex, p_IndexBuffer->indices.size());

                VkSemaphore waitSemaphores[] = {
                    p_SyncObjects->computeFinishedSemaphores[currentFrame],
                    p_SyncObjects->imageAvailableSemaphores[currentFrame]
                };
                VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

                queuesubmit.submit(
                    p_LogicalDevice->graphicsQueue,
                    p_SyncObjects->inFlightFences[currentFrame],
                    &p_CommandBuffers->commandBuffers[currentFrame],
                    &p_SyncObjects->renderFinishedSemaphores[currentFrame],
                    2,
                    waitSemaphores,
                    waitStages
                );

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &p_SyncObjects->renderFinishedSemaphores[currentFrame];
        presentInfo.swapchainCount = 1;
        VkSwapchainKHR swapChains[] = { p_SwapChain->swapChain };
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        result = vkQueuePresentKHR(p_LogicalDevice->presentQueue, &presentInfo);

        if (
            result == VK_ERROR_OUT_OF_DATE_KHR || 
            result == VK_SUBOPTIMAL_KHR || 
            framebufferResized ||
            config->newSwapChainNeeded
            ) {
            framebufferResized = false;
            recreateSwapChain();
            config->newSwapChainNeeded = false;
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
      
        currentFrame = (currentFrame + 1) % config->maxFramesInFlight;
    }

    void updateComputeUniformBuffer(uint32_t currentImage) {
        config->ubo.octreeSize = config->octreeWidth;
        config->ubo.octreeMaxDepth = config->octreeDepth;
        config->ubo.deltaTime = config->lastFrameTime * 2.0f;

        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, 0.0001f * config->ubo.deltaTime, glm::vec3(0, 1, 1));
        config->ubo.sunDir = glm::vec3(rotationMat * glm::vec4(config->ubo.sunDir, 1.0));

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec3 camLeft = glm::cross(config->ubo.camUp, config->ubo.camDir);
            config->ubo.camPos += config->ubo.deltaTime * config->moveSpeed * camLeft;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::vec3 camLeft = glm::cross(config->ubo.camUp, config->ubo.camDir);
            config->ubo.camPos -= config->ubo.deltaTime * config->moveSpeed * camLeft;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            config->ubo.camPos += config->ubo.deltaTime * config->moveSpeed * config->ubo.camDir;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            config->ubo.camPos -= config->ubo.deltaTime * config->moveSpeed * config->ubo.camDir;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            config->ubo.camPos += config->ubo.deltaTime * config->moveSpeed * glm::vec3(0, 1, 0);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            config->ubo.camPos += config->ubo.deltaTime * config->moveSpeed * glm::vec3(0, -1, 0);

        if (config->cursorActive && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            float deltaX = 0;
            float deltaY = 0;

            glfwGetCursorPos(window, &config->cursorX, &config->cursorY);
            if (!(config->prevCursorX == -999 && config->prevCursorY == -999)) {
                deltaX = config->prevCursorX - config->cursorX;
                deltaY = config->prevCursorY - config->cursorY;
            }
            config->prevCursorX = config->cursorX;
            config->prevCursorY = config->cursorY;

            glm::mat4 rotationMat(1);
            glm::vec3 camLeft = glm::cross(config->ubo.camUp, config->ubo.camDir);

            rotationMat = glm::rotate(rotationMat, config->rotateSpeed * deltaX, glm::vec3(0, 1, 0));

            rotationMat = glm::rotate(rotationMat, -config->rotateSpeed * deltaY, camLeft);

            //config->ubo.camDir = glm::vec3(rotationMat * glm::vec4(config->ubo.camDir, 1.0));
            //config->ubo.camUp = glm::vec3(rotationMat * glm::vec4(config->ubo.camUp, 1.0));

            //rotationMat = glm::mat4(1);
            config->ubo.camDir = glm::vec3(rotationMat * glm::vec4(config->ubo.camDir, 1.0));
            config->ubo.camUp = glm::vec3(rotationMat * glm::vec4(config->ubo.camUp, 1.0));
        }
        else {
            config->prevCursorX = -999;
            config->prevCursorY = -999;
        }

        config->ubo.height = p_SwapChain->swapChainExtent.height / config->downScaleFactor;
        config->ubo.width = p_SwapChain->swapChainExtent.width / config->downScaleFactor;
        p_ComputeUBO->update(currentImage, &config->ubo);
    }

    void cleanupSwapChain() {
        p_FrameBuffers->destroy();
        p_ImageViews->destroySwapChainImageViews();
        p_SwapChain->destroySwapChain();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
        p_LogicalDevice->waitIdle();

        p_Gui->vulkanShutdown();
        cleanupSwapChain();

        p_DepthComputeDescriptorSets->destroy();
        p_DepthComputeDescriptorPool->destroy();

        p_ComputeDescriptorSets->destroy();
        p_ComputeDescriptorPool->destroy();

        p_GraphicsDescriptorSets->destroy();
        p_GraphicsDescriptorPool->destroy();

        p_DepthImage->destroy();
        p_CanvasImage->destroy();
        p_CanvasSampler->destroy();

        p_SwapChain->updateSwapChainSupport(
            p_PhysicalDevice->getSwapChainSupportDetails()
        );
        p_SwapChain->create();
        p_ImageViews->createSwapChainImageViews(
            p_SwapChain->swapChainImages, 
            p_SwapChain->swapChainImageFormat
        );
       
        p_FrameBuffers->create(NULL, NULL);
        
        p_DepthImage->create(p_SwapChain->swapChainExtent);

        p_CanvasImage->create(p_SwapChain->swapChainExtent);
        p_CanvasSampler->create();

        p_ComputeDescriptorPool->bindUniformBuffer(computeUniformBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageBuffer(computeSSBOBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageBuffer(computeChunkInfoSSBOBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageImage(computeDepthImageBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageImage(computeImageBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->create();

        p_DepthComputeDescriptorPool->bindUniformBuffer(depthComputeUniformBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageBuffer(depthComputeSSBOBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageBuffer(depthComputeChunkInfoSSBOBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageImage(depthComputeDepthImageBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->bindStorageImage(depthComputeImageBinding, config->maxFramesInFlight);
        p_DepthComputeDescriptorPool->create();

        p_DepthComputeDescriptorSets->reInit();
        p_DepthComputeDescriptorSets->bindUniformBuffer(
            depthComputeUniformBinding,
            p_ComputeUBO->uniformBuffers,
            2 * sizeof(ComputeUniformBufferObject)
        );
        p_DepthComputeDescriptorSets->bindStorageBuffer(
            depthComputeSSBOBinding,
            p_OctreeSSBO->shaderStorageBuffers,
            sizeof(unsigned int) * octreeArray.size()
        );
        p_DepthComputeDescriptorSets->bindStorageBuffer(
            depthComputeChunkInfoSSBOBinding,
            p_ChunkInfoSSBO->shaderStorageBuffers,
            sizeof(ChunkInfo) * chunkInfos.size()
        );
        p_DepthComputeDescriptorSets->bindStorageImage(
            depthComputeDepthImageBinding,
            p_DepthImage->storageImageViews
        );
        p_DepthComputeDescriptorSets->bindStorageImage(
            depthComputeImageBinding,
            p_CanvasImage->storageImageViews
        );
        p_DepthComputeDescriptorSets->create();

        p_ComputeDescriptorSets->reInit();
        p_ComputeDescriptorSets->bindUniformBuffer(
            computeUniformBinding,
            p_ComputeUBO->uniformBuffers,
            2*sizeof(ComputeUniformBufferObject)
        );
        p_ComputeDescriptorSets->bindStorageBuffer(
            computeSSBOBinding,
            p_OctreeSSBO->shaderStorageBuffers,
            sizeof(unsigned int) * octreeArray.size()
        );
        p_ComputeDescriptorSets->bindStorageBuffer(
            computeChunkInfoSSBOBinding,
            p_ChunkInfoSSBO->shaderStorageBuffers,
            sizeof(ChunkInfo) * chunkInfos.size()
        );
        p_ComputeDescriptorSets->bindStorageImage(
            computeDepthImageBinding,
            p_DepthImage->storageImageViews
        );
        p_ComputeDescriptorSets->bindStorageImage(
            computeImageBinding,
            p_CanvasImage->storageImageViews
        );
        p_ComputeDescriptorSets->create();

        p_GraphicsDescriptorPool->bindSampler(fragmentSamplerBinding, config->maxFramesInFlight);
        p_GraphicsDescriptorPool->bindSampler(imguiSamplerBinding, config->maxFramesInFlight);
        p_GraphicsDescriptorPool->create();

        p_GraphicsDescriptorSets->reInit();
        p_GraphicsDescriptorSets->bindSampler(
            fragmentSamplerBinding,
            p_CanvasImage->storageImageViews,
            p_CanvasSampler->imageSamplers
        );
        p_GraphicsDescriptorSets->create();

        p_Gui->reInit(p_GraphicsDescriptorPool->descriptorPool);
        
        p_CommandBuffers->attachFrameBuffers(p_FrameBuffers->frameBuffers);
        p_CommandBuffers->attachDescriptorSets(p_GraphicsDescriptorSets->descriptorSets);
        p_CommandBuffers->setExtent(p_SwapChain->swapChainExtent);
        p_CommandBuffers->create();

        p_ComputeCommandBuffers->attachDescriptorSets(p_ComputeDescriptorSets->descriptorSets);
        
        p_ComputeCommandBuffers->setGroupSize(
            (p_SwapChain->swapChainExtent.width * p_SwapChain->swapChainExtent.height) -
            ((p_SwapChain->swapChainExtent.width / 2) * (p_SwapChain->swapChainExtent.height / 2))
        );
        

        /*
        p_ComputeCommandBuffers->setExtent(
            p_SwapChain->swapChainExtent.width,
            p_SwapChain->swapChainExtent.height
        );
        */
        p_ComputeCommandBuffers->create();

        p_DepthComputeCommandBuffers->attachDescriptorSets(p_DepthComputeDescriptorSets->descriptorSets);
        p_DepthComputeCommandBuffers->setExtent(p_SwapChain->swapChainExtent.width / 2, p_SwapChain->swapChainExtent.height / 2);
        p_DepthComputeCommandBuffers->create();
        
        /*
        p_CanvasImage->transitionImageLayout(
            p_CanvasImage->storageImages[currentFrame],
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_GENERAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        */
    }

    void cleanup() {
        p_LogicalDevice->waitIdle();

        p_Gui->destroy();

        p_SyncObjects->destroy();

        p_FrameBuffers->destroy();

        //p_ColorResources->destroy();
        //p_DepthResources->destroy();

        p_GraphicsDescriptorPool->destroy();
        p_ComputeDescriptorPool->destroy();
        p_DepthComputeDescriptorPool->destroy();

        p_IndexBuffer->destroy();
        p_VertexBuffer->destroy();
        p_ComputeUBO->destroy();
        p_CanvasSampler->destroy();
        p_CanvasImage->destroy();
        p_OctreeSSBO->destroy();
        p_ChunkInfoSSBO->destroy();
        p_DepthImage->destroy();


        p_CommandPool->destroy();

        p_ComputePipeline->destroy();
        p_DepthComputePipeline->destroy();
        p_GraphicsPipeline->destroy();

        p_ComputeDescriptorSetLayout->destroy();
        p_DepthComputeDescriptorSetLayout->destroy();
        p_GraphicsDescriptorSetLayout->destroy();

        p_RenderPass->destroy();
        p_ImageViews->destroySwapChainImageViews();
        p_SwapChain->destroySwapChain();

        p_LogicalDevice->destroy();
        p_Surface->destroy();
        p_Instance->DestroyDebugUtilsMessengerEXT(nullptr);
        p_Instance->destroy();
    }

    void createOctree() {

        int numChunks = 0;
        int chunkIndex = 0;
        int minX = 0;
        int maxX = 8;
        int minY = 0;
        int maxY = 2;
        int minZ = 0;
        int maxZ = 8;
        for (int y = minY; y < maxY; y++) {
            for (int x = minX; x < maxX; x++) {
                for (int z = minZ; z < maxZ; z++) {
                    int LOD = 0;
                    if (x > 6 || z > 6) {
                        LOD = 1;
                    }
                    if (x > 10 || z > 10) {
                        LOD = 2;
                    }
                    BitmaskOctree bitmaskOctree(
                        x * config->octreeWidth, 
                        y * config->octreeWidth, 
                        z * config->octreeWidth, 
                        config->octreeDepth, 
                        LOD
                    );

                    if (bitmaskOctree.bitMaskArray[0] != 0) {
                        octreeArray.insert(
                            octreeArray.end(), 
                            bitmaskOctree.bitMaskArray.begin(), 
                            bitmaskOctree.bitMaskArray.end()
                        );

                        ChunkInfo chunkInfo{};
                        chunkInfo.x = x * config->octreeWidth;
                        chunkInfo.y = y * config->octreeWidth;
                        chunkInfo.z = z * config->octreeWidth;
                        chunkInfo.index = chunkIndex;
                        chunkInfos.push_back(chunkInfo);

                        numChunks++;
                        chunkIndex += bitmaskOctree.bitMaskArray.size();
                    }
                }
            }
        }
        config->ubo.numChunks = numChunks;

        config->ubo.minX = minX * config->octreeWidth;
        config->ubo.minY = minY * config->octreeWidth;
        config->ubo.minZ = minZ * config->octreeWidth;

        config->ubo.maxX = (maxX + 1) * config->octreeWidth;
        config->ubo.maxY = (maxY + 1) * config->octreeWidth;
        config->ubo.maxZ = (maxZ + 1) * config->octreeWidth;
    }
};
