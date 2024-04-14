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
    std::shared_ptr<RenderingSettings> renderingSettings;

    GLFWwindow* window = nullptr;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    std::shared_ptr<Instance> p_Instance;
    std::shared_ptr<Surface> p_Surface;
    std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
    std::shared_ptr<LogicalDevice> p_LogicalDevice;
    std::shared_ptr<SwapChain> p_SwapChain;

    void initSettings() {
        renderingSettings = std::make_shared<RenderingSettings>();
        renderingSettings->enableValidationLayers = enableValidationLayers;
        renderingSettings->deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        renderingSettings->validationLayers = { "VK_LAYER_KHRONOS_validation" };
        renderingSettings->computeEnabled = true;
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(renderingSettings->WIDTH, renderingSettings->HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<TestRewrite*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        p_Instance = std::make_shared<Instance>(renderingSettings);
        p_Instance->create();

        p_Surface = std::make_shared<Surface>(
            p_Instance->instance, 
            window
        );

        p_PhysicalDevice = std::make_shared<PhysicalDevice>(
            p_Instance->instance, 
            p_Surface->surface, 
            renderingSettings
        );
        p_PhysicalDevice->create();

        p_LogicalDevice = std::make_shared<LogicalDevice>(
            p_PhysicalDevice,
            renderingSettings
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

    }

    void cleanup() {
        p_SwapChain->destroySwapChain();

        p_LogicalDevice->destroy();

        p_Surface->destroy();

        p_Instance->DestroyDebugUtilsMessengerEXT(nullptr);
        p_Instance->destroy();
    }
};
