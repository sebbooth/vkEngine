#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION

#define TINYOBJLOADER_IMPLEMENTATION

#include <iomanip> // For std::setw

#include "VkClasses.h"


const uint32_t screenWidth = 800;
const uint32_t screenHeight = 600;
const std::string MODEL_PATH = "assets/models/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "assets/models/viking_room/viking_room.png";
const int maxFramesInFlight = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const bool enableValidationLayers = true;

/*
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
*/



class GraphicsApp {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window = nullptr;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    std::shared_ptr<Instance> p_Instance;
        //std::shared_ptr<DebugMessenger> p_DebugMessenger;
    std::shared_ptr<Surface> p_Surface;
    std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
    std::shared_ptr<LogicalDevice> p_LogicalDevice;
    std::shared_ptr<SwapChain> p_SwapChain;
    std::shared_ptr<ImageViews> p_ImageViews;
    std::shared_ptr<RenderPass> p_RenderPass;
    std::shared_ptr<DescriptorSetLayout> p_GraphicsDescriptorSetLayout;
    std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
    std::shared_ptr<CommandPool> p_CommandPool;
        //std::shared_ptr<ColorResources> p_ColorResources;
        //std::shared_ptr<DepthResources> p_DepthResources;
    std::shared_ptr<FrameBuffers> p_FrameBuffers;
    std::shared_ptr<TextureImage> p_TextureImage;
    std::shared_ptr<TextureSampler> p_TextureSampler;
    std::shared_ptr<Model> p_Model;
    std::shared_ptr<VertexBuffer> p_VertexBuffer;
    std::shared_ptr<IndexBuffer> p_IndexBuffer;
    std::shared_ptr<UniformBuffers> p_UniformBuffers;
    std::shared_ptr<DescriptorPool> p_DescriptorPool;
    std::shared_ptr<DescriptorSets> p_DescriptorSets;


    std::shared_ptr<Gui> p_Gui;

    std::shared_ptr<CommandBuffers> p_CommandBuffer;
    std::shared_ptr<SyncObjects> p_SyncObjects;

    bool guiEnabled = true;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(screenWidth, screenHeight, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
       
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<GraphicsApp*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        p_Instance = std::make_shared<Instance>(enableValidationLayers, validationLayers, deviceExtensions);
        p_Instance->create();
        //p_DebugMessenger = std::make_shared<DebugMessenger>(p_Instance);

        p_Surface = std::make_shared<Surface>(p_Instance, window);

        p_PhysicalDevice = std::make_shared<PhysicalDevice>(p_Surface);
        p_PhysicalDevice->msaaEnabled = false;
        p_PhysicalDevice->create();

        p_LogicalDevice = std::make_shared<LogicalDevice>(p_PhysicalDevice);
        p_LogicalDevice->wireFrameEnabled = false;
        p_LogicalDevice->create();

        p_SwapChain = std::make_shared<SwapChain>(p_LogicalDevice);
        p_SwapChain->create();

        p_ImageViews = std::make_shared<ImageViews>(p_SwapChain);
        p_ImageViews->create();

        p_RenderPass = std::make_shared<RenderPass>(p_ImageViews);
        p_RenderPass->depthEnabled = true;
        p_RenderPass->create();

        p_GraphicsDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(p_RenderPass);
        p_GraphicsDescriptorSetLayout->guiEnabled = guiEnabled;
        p_GraphicsDescriptorSetLayout->uboEnabled = true;
        p_GraphicsDescriptorSetLayout->samplerEnabled = true;
        p_GraphicsDescriptorSetLayout->create();

        p_GraphicsPipeline = std::make_shared<GraphicsPipeline>(p_GraphicsDescriptorSetLayout);
        p_GraphicsPipeline->cullBackFaces = false;
        p_GraphicsPipeline->displayNormals = false;
        p_GraphicsPipeline->create();

        p_CommandPool = std::make_shared<CommandPool>(p_GraphicsPipeline);
        p_CommandPool->create();

        p_FrameBuffers = std::make_shared<FrameBuffers>(p_CommandPool);
        p_FrameBuffers->create();

        p_Model = std::make_shared<Model>();

        p_VertexBuffer = std::make_shared<VertexBuffer>(p_FrameBuffers);
        p_VertexBuffer->create(p_Model->vertices);

        p_IndexBuffer = std::make_shared<IndexBuffer>(p_VertexBuffer);
        p_IndexBuffer->create(p_Model->indices);
        
        p_DescriptorPool = std::make_shared<DescriptorPool>(p_FrameBuffers);
        p_DescriptorPool->create();

        p_DescriptorSets = std::make_shared<DescriptorSets>(p_DescriptorPool);
        p_DescriptorSets->create();

        p_Gui = std::make_shared<Gui>(p_DescriptorSets);

        p_CommandBuffer = std::make_shared<CommandBuffers>(p_DescriptorSets);
        p_CommandBuffer->attachVertexBuffer(p_VertexBuffer);
        p_CommandBuffer->attachIndexBuffer(p_IndexBuffer);
        p_CommandBuffer->attachGui(p_Gui);
        p_CommandBuffer->create();
        
        p_SyncObjects = std::make_shared<SyncObjects>(p_CommandBuffer);
        p_SyncObjects->create();
        

        p_Gui->init();
    }

    void rebuild() {
        bool prevDepthEnabled = p_Gui->depthEnabled;
        bool prevWireFrameEnabled = p_Gui->wireFrameEnabled;
        bool prevSamplerEnabled = p_Gui->samplerEnabled;
        bool prevMsaaEnabled = p_Gui->msaaEnabled;
        bool prevDisplayNormals = p_Gui->displayNormalsEnabled;
        bool prevCullBackFaces = p_Gui->cullBackFacesEnabled;

        p_LogicalDevice->waitIdle();
        p_Gui->destroy();

        cleanupSwapChain();

        p_GraphicsPipeline->destroy();
        p_RenderPass->destroy();
        p_DescriptorPool->destroy();
        p_GraphicsDescriptorSetLayout->destroy();
        p_IndexBuffer->destroy();
        p_VertexBuffer->destroy();
        p_SyncObjects->destroy();
        p_CommandPool->destroy();
        p_LogicalDevice->destroy();


        p_PhysicalDevice->msaaEnabled = prevMsaaEnabled;
        p_PhysicalDevice->create();
        p_LogicalDevice->wireFrameEnabled = prevWireFrameEnabled;
        p_LogicalDevice->create();
        p_SwapChain->create();
        p_ImageViews->create();
        p_RenderPass->depthEnabled = prevDepthEnabled;
        p_RenderPass->create();
        p_GraphicsDescriptorSetLayout->guiEnabled = guiEnabled;
        p_GraphicsDescriptorSetLayout->uboEnabled = true;
        p_GraphicsDescriptorSetLayout->samplerEnabled = prevSamplerEnabled;
        p_GraphicsDescriptorSetLayout->create();
        p_GraphicsPipeline->cullBackFaces = prevCullBackFaces;
        p_GraphicsPipeline->displayNormals = prevDisplayNormals;
        p_GraphicsPipeline->create();
        p_CommandPool->create();
        p_FrameBuffers->create();
        p_VertexBuffer->create(p_Model->vertices);
        p_IndexBuffer->create(p_Model->indices);
        p_DescriptorPool->create();
        p_DescriptorSets->create();
        p_CommandBuffer->attachVertexBuffer(p_VertexBuffer);
        p_CommandBuffer->attachIndexBuffer(p_IndexBuffer);
        p_CommandBuffer->attachGui(p_Gui);
        p_CommandBuffer->create();
        p_SyncObjects->create();

        p_Gui->init();
    }

    void recreateFromRenderPass() {

        //glfwWaitEvents();
        p_LogicalDevice->waitIdle();

        p_FrameBuffers->destroy();
        p_SwapChain->destroyFramebuffers();
        p_ImageViews->destroyImageViews();
        p_SwapChain->destroySwapChain();
        p_GraphicsPipeline->destroy();
        p_RenderPass->destroy();
      
        p_RenderPass->depthEnabled = p_Gui->depthEnabled;
        p_RenderPass->create();
        p_GraphicsPipeline->create();
        p_SwapChain->create();
        p_ImageViews->create();
        p_FrameBuffers->create();
    }

    void mainLoop() {

       
        long long fps;
        long long microsecPerSec = 1000000;
        std::cout << std::endl;

        while (!glfwWindowShouldClose(window)) {
            auto start = std::chrono::high_resolution_clock::now();

            glfwPollEvents();
            drawFrame();

            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            fps = microsecPerSec / std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            std::cout << "\r" << "FPS: " << std::setw(10) << fps << std::flush;
        }
        std::cout << std::endl;
        p_LogicalDevice->waitIdle();
    }

    void drawFrame() {
        p_SyncObjects->waitForFences(currentFrame, VK_TRUE, UINT64_MAX);

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

        p_DescriptorPool->update(currentFrame);
        p_CommandBuffer->recordBuffer(currentFrame, imageIndex);

        vkResetFences(p_LogicalDevice->device, 1, &p_SyncObjects->inFlightFences[currentFrame]);

        //p_CommandBuffer->resetBuffer(currentFrame);
        //p_CommandBuffer->recordBuffer(currentFrame, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { p_SyncObjects->imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &p_CommandBuffer->commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { p_SyncObjects->renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(p_LogicalDevice->graphicsQueue, 1, &submitInfo, p_SyncObjects->inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { p_SwapChain->swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(p_LogicalDevice->presentQueue, &presentInfo);

        bool needRebuild = p_Gui->hasChanged();
        if (needRebuild) {
            rebuild();
        }

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % maxFramesInFlight;
    }

    void cleanup() {

        p_Gui->destroy();

        cleanupSwapChain();

        p_GraphicsPipeline->destroy();
        p_RenderPass->destroy();
        p_DescriptorPool->destroy();
        p_GraphicsDescriptorSetLayout->destroy();
        p_IndexBuffer->destroy();
        p_VertexBuffer->destroy();
        p_SyncObjects->destroy();
        p_CommandPool->destroy();
        p_LogicalDevice->destroy();

        if (enableValidationLayers) {
            p_Instance->DestroyDebugUtilsMessengerEXT(nullptr);
        }

        p_Surface->destroy();
        p_Instance->destroy();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void cleanupSwapChain() {
        p_FrameBuffers->destroy();
        p_SwapChain->destroyFramebuffers();
        p_ImageViews->destroyImageViews();
        p_SwapChain->destroySwapChain();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        p_LogicalDevice->waitIdle();

        cleanupSwapChain();

        p_SwapChain->create();
        p_ImageViews->create();
        p_FrameBuffers->create();
    }
};
