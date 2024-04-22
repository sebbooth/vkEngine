#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "VkClassesIndex.h"

#include <random>
#include "Octree.h"
#include "Perlin.h"
#include "SimplexNoise.h"

const bool enableValidationLayers = true;

const std::vector<Vertex> quadVertices = {
        {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

std::vector<uint32_t> quadIndices = {
    0,1,2,
    2,3,0
};

class TestRewrite {

public:
    void run() {
        initSettings();
        for (int i = 0; i < 1; i++) {
            createOctree();

        }
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
    std::shared_ptr<DescriptorSetLayout> p_ComputeDescriptorSetLayout;
    uint32_t vertexUniformBinding;
    uint32_t fragmentSamplerBinding;
    uint32_t imguiSamplerBinding;
    uint32_t computeUniformBinding;
    uint32_t computeSSBOBinding;
    uint32_t computeImageBinding;

    std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
    std::shared_ptr<ComputePipeline> p_ComputePipeline;

    std::shared_ptr<CommandPool> p_CommandPool;

    std::shared_ptr<SSBO> p_OctreeSSBO;
    std::shared_ptr<StorageImage> p_CanvasImage;
    std::shared_ptr<Sampler> p_CanvasSampler;
    std::shared_ptr<UniformBuffer> p_ComputeUBO;
    std::shared_ptr<UniformBuffer> p_VertexUBO;
    std::shared_ptr<VertexBuffer> p_VertexBuffer;
    std::shared_ptr<IndexBuffer> p_IndexBuffer;

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

    std::shared_ptr<SyncObjects> p_SyncObjects;

    void initSettings() {
        config = std::make_shared<VkConfig>();
        config->enableValidationLayers = enableValidationLayers;
        config->deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        config->validationLayers = { "VK_LAYER_KHRONOS_validation" };
        config->computeEnabled = true;
        config->msaaEnabled = false;
        config->depthStencilEnabled = false;
        config->downScaleFactor = 1;
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(config->screenWidth, config->screenHeight, "VkEngine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<TestRewrite*>(glfwGetWindowUserPointer(window));
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

        p_ComputeDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(p_LogicalDevice->device);

        // Note: Must do DSL bindings in order of bindings in shader code
        // Should be fine to do bindings in any order for rest of code (Descriptor Pools and Sets)
        // since binding functions will be called with the binding indices returned here
        computeUniformBinding = p_ComputeDescriptorSetLayout->bindCompUniformBuffer();  //0
        computeSSBOBinding = p_ComputeDescriptorSetLayout->bindCompStorageBuffer();     //1
        computeImageBinding = p_ComputeDescriptorSetLayout->bindCompStorageImage();     //2
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

        p_ComputePipeline = std::make_shared<ComputePipeline>(
            p_LogicalDevice->device,
            p_ComputeDescriptorSetLayout->descriptorSetLayout
        );
        std::string computeShaderFile = "shaders/old_raycast_comp.spv";
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
        p_OctreeSSBO = std::make_shared<SSBO>(
            p_LogicalDevice,
            p_CommandPool,
            config
        );
        p_OctreeSSBO->uploadData(
            sizeof(OctreeNode) * octree.octreeArray.size(),
            octree.octreeArray.data()
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
        p_ComputeUBO->create(sizeof(ComputeUniformBufferObject));
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
        p_ComputeDescriptorPool = std::make_shared<DescriptorPool>(
            p_LogicalDevice->device,
            config
        );
        p_ComputeDescriptorPool->bindUniformBuffer(computeUniformBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageBuffer(computeSSBOBinding, config->maxFramesInFlight);
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
            sizeof(ComputeUniformBufferObject)
        );
        p_ComputeDescriptorSets->bindStorageBuffer(
            computeSSBOBinding,
            p_OctreeSSBO->shaderStorageBuffers,
            sizeof(OctreeNode) * octree.octreeArray.size()
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

        p_ComputeCommandBuffers = std::make_shared<ComputeCommandBuffers>(
            p_LogicalDevice->device,
            p_CommandPool->commandPool,
            config
        );
        p_ComputeCommandBuffers->attachComputePipeline(p_ComputePipeline->computePipeline, p_ComputePipeline->computePipelineLayout);
        p_ComputeCommandBuffers->attachDescriptorSets(p_ComputeDescriptorSets->descriptorSets);
        p_ComputeCommandBuffers->setExtent(p_SwapChain->swapChainExtent);
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
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Compute submission        
                vkWaitForFences(p_LogicalDevice->device, 1, &p_SyncObjects->computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

                updateComputeUniformBuffer(currentFrame);

                vkResetFences(p_LogicalDevice->device, 1, &p_SyncObjects->computeInFlightFences[currentFrame]);

                vkResetCommandBuffer(p_ComputeCommandBuffers->commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
                p_ComputeCommandBuffers->recordBuffer(
                    p_ComputeCommandBuffers->commandBuffers[currentFrame], 
                    currentFrame
                );

                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &p_ComputeCommandBuffers->commandBuffers[currentFrame];
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &p_SyncObjects->computeFinishedSemaphores[currentFrame];

                if (vkQueueSubmit(p_LogicalDevice->computeQueue, 1, &submitInfo, p_SyncObjects->computeInFlightFences[currentFrame]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to submit compute command buffer!");
                };

        // Graphics submission
        vkWaitForFences(p_LogicalDevice->device, 1, &p_SyncObjects->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

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

        vkResetFences(p_LogicalDevice->device, 1, &p_SyncObjects->inFlightFences[currentFrame]);
        
        /*
        p_CanvasImage->transitionImageLayout(
            p_CanvasImage->storageImages[currentFrame], 
            VK_FORMAT_R8G8B8A8_UNORM, 
            VK_IMAGE_LAYOUT_GENERAL, 
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        */

        //p_DescriptorPool->update(currentFrame);
        p_CommandBuffers->recordBufferIndexed(currentFrame, imageIndex, p_IndexBuffer->indices.size());

            VkSemaphore waitSemaphores[] = { p_SyncObjects->computeFinishedSemaphores[currentFrame], p_SyncObjects->imageAvailableSemaphores[currentFrame] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            submitInfo.waitSemaphoreCount = 2;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &p_CommandBuffers->commandBuffers[currentFrame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &p_SyncObjects->renderFinishedSemaphores[currentFrame];

            if (vkQueueSubmit(p_LogicalDevice->graphicsQueue, 1, &submitInfo, p_SyncObjects->inFlightFences[currentFrame]) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }
 
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &p_SyncObjects->renderFinishedSemaphores[currentFrame];

        VkSwapchainKHR swapChains[] = { p_SwapChain->swapChain };
        presentInfo.swapchainCount = 1;
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
        /*
        p_CanvasImage->transitionImageLayout(
            p_CanvasImage->storageImages[currentFrame],
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_GENERAL
        );
        */
        currentFrame = (currentFrame + 1) % config->maxFramesInFlight;
    }

    void updateComputeUniformBuffer(uint32_t currentImage) {
        config->ubo.octreeSize = config->octreeWidth;
        config->ubo.octreeMaxDepth = config->octreeDepth;
        config->ubo.deltaTime = config->lastFrameTime * 2.0f;

        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, 0.0001f * config->ubo.deltaTime, glm::vec3(0, 1, 0));
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
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            config->ubo.camPos += config->ubo.deltaTime * config->moveSpeed * config->ubo.camUp;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            config->ubo.camPos -= config->ubo.deltaTime * config->moveSpeed * config->ubo.camUp;

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
            rotationMat = glm::rotate(rotationMat, config->rotateSpeed * deltaX, config->ubo.camUp);
            config->ubo.camDir = glm::vec3(rotationMat * glm::vec4(config->ubo.camDir, 1.0));

            rotationMat = glm::mat4(1);
            glm::vec3 camLeft = glm::cross(config->ubo.camUp, config->ubo.camDir);
            rotationMat = glm::rotate(rotationMat, -config->rotateSpeed * deltaY, camLeft);
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

        p_ComputeDescriptorSets->destroy();
        p_ComputeDescriptorPool->destroy();

        p_GraphicsDescriptorSets->destroy();
        p_GraphicsDescriptorPool->destroy();

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
        
        p_CanvasImage->create(p_SwapChain->swapChainExtent);
        p_CanvasSampler->create();

        p_ComputeDescriptorPool->bindUniformBuffer(computeUniformBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageBuffer(computeSSBOBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->bindStorageImage(computeImageBinding, config->maxFramesInFlight);
        p_ComputeDescriptorPool->create();

        p_ComputeDescriptorSets->reInit();
        p_ComputeDescriptorSets->bindUniformBuffer(
            computeUniformBinding,
            p_ComputeUBO->uniformBuffers,
            sizeof(ComputeUniformBufferObject)
        );
        p_ComputeDescriptorSets->bindStorageBuffer(
            computeSSBOBinding,
            p_OctreeSSBO->shaderStorageBuffers,
            sizeof(OctreeNode) * octree.octreeArray.size()
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
        p_ComputeCommandBuffers->setExtent(p_SwapChain->swapChainExtent);
        p_ComputeCommandBuffers->create();
        
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

        p_IndexBuffer->destroy();
        p_VertexBuffer->destroy();
        p_ComputeUBO->destroy();
        p_CanvasSampler->destroy();
        p_CanvasImage->destroy();
        p_OctreeSSBO->destroy();

        p_CommandPool->destroy();

        p_ComputePipeline->destroy();
        p_GraphicsPipeline->destroy();

        p_ComputeDescriptorSetLayout->destroy();
        p_GraphicsDescriptorSetLayout->destroy();

        p_RenderPass->destroy();
        p_ImageViews->destroySwapChainImageViews();
        p_SwapChain->destroySwapChain();

        p_LogicalDevice->destroy();
        p_Surface->destroy();
        p_Instance->DestroyDebugUtilsMessengerEXT(nullptr);
        p_Instance->destroy();
    }

    Octree octree;
   
    void createOctree() {
        int depth = config->octreeDepth;
        int width = config->octreeWidth;

        Perlin perlinGenerator;
        perlinGenerator.layers = config->octreeDepth - 1;
        perlinGenerator.init_amp = 2;
        perlinGenerator.grid_size = width * 2;

        SimplexNoise simplexNoiseGenerator = SimplexNoise(8.0f, 3.0f, 3.0f, 0.1f);

        std::random_device rd;  // Seed for random number engine
        std::mt19937 gen(rd()); // Mersenne Twister random number engine
        std::uniform_int_distribution<> distribution(-config->octreeWidth / 20, config->octreeWidth / 20); // Range from 1 to 100
        std::uniform_int_distribution<> materialDist(0, 5); // Range from 1 to 100

        std::vector<Voxel> testRandomVoxels;

        std::cout << "Generating Random Voxels... " << std::endl;


        for (int x = 0; x < width; x++) {
            for (int y = 0; y < width; y++) {
                for (int z = 0; z < width; z++) {
                    if (config->terrain) {
                        float perlinVal = perlinGenerator.perlin2D(x, z);
                        float simplex = simplexNoiseGenerator.fractal(5, (float)x / width, (float)y / width, (float)z / width);

                        int yThreshold = (int)(perlinVal * width);
                        if (y < yThreshold && simplex < 0.5f) {
                            Voxel voxel{};
                            voxel.x = x;
                            voxel.y = y;
                            voxel.z = z;
                            voxel.mat = -2 - materialDist(gen);   // -2 - -7 grass
                            if (y < yThreshold - 6) {
                                voxel.mat = -8 - materialDist(gen);   // -8 - -13 stone
                            }
                            if (y > distribution(gen) + (width * 0.55)) {
                                voxel.mat = -8 - materialDist(gen);
                            }
                            if (y > distribution(gen) + (width * 0.70)) {
                                voxel.mat = -14 - materialDist(gen);  // -14 - -19 snow
                            }
                            testRandomVoxels.push_back(voxel);
                        }
                    }
                    else {
                        float simplex = simplexNoiseGenerator.fractal(5, (float)x / width, (float)y / width, (float)z / width);
                        if (simplex > 0.7f) {
                            Voxel voxel{};
                            voxel.x = x;
                            voxel.y = y;
                            voxel.z = z;
                            voxel.mat = -2;   // -2 - -7 grass
                            testRandomVoxels.push_back(voxel);
                        }
                    }
                }
            }
        }

        std::cout << "Generating SVO Chunk Size: " << width << " x " << width << " x " << width << std::endl;
        octree = Octree(testRandomVoxels, depth);
        std::cout << "Octree Size: " << octree.octreeArray.size() << std::endl;
    }
};
