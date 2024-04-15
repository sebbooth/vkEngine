#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "VkClassesReWrite.h"
#include "RenderingSettings.h"


const bool enableValidationLayers = true;

class TestRewrite {

public:
    void run() {
        initSettings();
        initWindow();
        initVulkan();
        cleanup();
    }

private:
    std::shared_ptr<RenderingSettings> RS;

    GLFWwindow* window = nullptr;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    std::shared_ptr<Instance> p_Instance;
    std::shared_ptr<Surface> p_Surface;
    std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
    std::shared_ptr<LogicalDevice> p_LogicalDevice;
    std::shared_ptr<SwapChain> p_SwapChain;
    std::shared_ptr<ImageViews> p_ImageViews;
    std::shared_ptr<RenderPass> p_RenderPass;

    std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout;
    std::shared_ptr<ComputeDescriptorSetLayout> p_ComputeDescriptorSetLayout;

    std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;

    void initSettings() {
        RS = std::make_shared<RenderingSettings>();
        RS->enableValidationLayers = enableValidationLayers;
        RS->deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        RS->validationLayers = { "VK_LAYER_KHRONOS_validation" };
        RS->computeEnabled = true;
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(RS->WIDTH, RS->HEIGHT, "VkEngine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<TestRewrite*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        p_Instance = std::make_shared<Instance>(RS);
        p_Instance->create();

        p_Surface = std::make_shared<Surface>(
            p_Instance->instance, 
            window
        );

        p_PhysicalDevice = std::make_shared<PhysicalDevice>(
            p_Instance->instance, 
            p_Surface->surface, 
            RS
        );
        p_PhysicalDevice->create();

        p_LogicalDevice = std::make_shared<LogicalDevice>(
            p_PhysicalDevice,
            RS
        );
        p_LogicalDevice->create();

        p_SwapChain = std::make_shared<SwapChain>(
            window,
            p_Surface->surface,
            p_LogicalDevice->device,
            p_PhysicalDevice->queueFamilies,
            p_PhysicalDevice->getSwapChainSupportDetails()
        );
        p_SwapChain->create();

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
            RS
        );
        p_RenderPass->create();


        p_DescriptorSetLayout = std::make_shared<DescriptorSetLayout>(
            p_LogicalDevice->device,
            RS
        );
        p_DescriptorSetLayout->create();

        p_ComputeDescriptorSetLayout = std::make_shared<ComputeDescriptorSetLayout>(p_LogicalDevice->device);
        uint32_t computeUniformBinding = p_ComputeDescriptorSetLayout->bindUniformBuffer();
        uint32_t computeSSBOBinding = p_ComputeDescriptorSetLayout->bindStorageBuffer();
        uint32_t computeImageBinding = p_ComputeDescriptorSetLayout->bindStorageImage();
        p_ComputeDescriptorSetLayout->create();

        
        p_GraphicsPipeline = std::make_shared<GraphicsPipeline>(
            p_PhysicalDevice->msaaSamples,
            p_LogicalDevice->device,
            p_SwapChain->swapChainExtent,
            p_RenderPass->renderPass,
            p_DescriptorSetLayout->descriptorSetLayout,
            RS
        );
        std::string vertexShaderFile = "shaders/vertNoUBO.spv";
        std::string fragmentShaderFile = "shaders/frag.spv";
        p_GraphicsPipeline->create(
            vertexShaderFile,
            fragmentShaderFile
        );


    }

    void cleanup() {
        p_GraphicsPipeline->destroy();

        p_ComputeDescriptorSetLayout->destroy();

        p_DescriptorSetLayout->destroy();

        p_RenderPass->destroy();

        p_ImageViews->destroySwapChainImageViews();

        p_SwapChain->destroySwapChain();

        p_LogicalDevice->destroy();

        p_Surface->destroy();

        p_Instance->DestroyDebugUtilsMessengerEXT(nullptr);
        p_Instance->destroy();
    }
};
