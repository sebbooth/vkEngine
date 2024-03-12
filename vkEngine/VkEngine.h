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


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::string MODEL_PATH = "assets/models/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "assets/models/viking_room/viking_room.png";
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class VkEngine {
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

    std::shared_ptr<Instance> instanceObj;
    std::shared_ptr<DebugMessenger> debugMessengerObj;
    std::shared_ptr<Surface> surfaceObj;
    std::shared_ptr<PhysicalDevice> physicalDeviceObj;
    std::shared_ptr<LogicalDevice> logicalDeviceObj;
    std::shared_ptr<SwapChain> swapChainObj;
    std::shared_ptr<ImageViews> imageViewsObj;
    std::shared_ptr<RenderPass> renderPassObj;
    std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutObj;
    std::shared_ptr<GraphicsPipeline> graphicsPipelineObj;
    std::shared_ptr<CommandPool> commandPoolObj;
    std::shared_ptr<ColorResources> colorResourcesObj;
    std::shared_ptr<DepthResources> depthResourcesObj;
    std::shared_ptr<FrameBuffers> frameBuffersObj;
    std::shared_ptr<TextureImage> textureImageObj;
    std::shared_ptr<TextureSampler> textureSamplerObj;
    std::shared_ptr<Model> modelObj;
    std::shared_ptr<VertexBuffer> vertexBufferObj;
    std::shared_ptr<IndexBuffer> indexBufferObj;
    std::shared_ptr<UniformBuffers> uniformBuffersObj;
    std::shared_ptr<DescriptorPool> descriptorPoolObj;
    std::shared_ptr<DescriptorSets> descriptorSetsObj;
    std::shared_ptr<CommandBuffers> commandBufferObj;
    std::shared_ptr<SyncObjects> syncObjectsObj;


    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<VkEngine*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        instanceObj = std::make_shared<Instance>(enableValidationLayers, validationLayers, deviceExtensions);
        debugMessengerObj = std::make_shared<DebugMessenger>(instanceObj);
        surfaceObj = std::make_shared<Surface>(instanceObj, window);
        physicalDeviceObj = std::make_shared<PhysicalDevice>(surfaceObj);
        logicalDeviceObj = std::make_shared<LogicalDevice>(physicalDeviceObj);
        swapChainObj = std::make_shared<SwapChain>(logicalDeviceObj);
        imageViewsObj = std::make_shared<ImageViews>(swapChainObj);
        renderPassObj = std::make_shared<RenderPass>(imageViewsObj);
        descriptorSetLayoutObj = std::make_shared<DescriptorSetLayout>(renderPassObj);
        graphicsPipelineObj = std::make_shared<GraphicsPipeline>(descriptorSetLayoutObj);
        commandPoolObj = std::make_shared<CommandPool>(graphicsPipelineObj);
        colorResourcesObj = std::make_shared<ColorResources>(commandPoolObj);
        depthResourcesObj = std::make_shared<DepthResources>(colorResourcesObj);
        frameBuffersObj = std::make_shared<FrameBuffers>(depthResourcesObj);
        textureImageObj = std::make_shared<TextureImage>(frameBuffersObj);
        textureSamplerObj = std::make_shared<TextureSampler>(textureImageObj);
        modelObj = std::make_shared<Model>(textureSamplerObj);
        vertexBufferObj = std::make_shared<VertexBuffer>(modelObj);
        indexBufferObj = std::make_shared<IndexBuffer>(vertexBufferObj);
        uniformBuffersObj = std::make_shared<UniformBuffers>(indexBufferObj);
        descriptorPoolObj = std::make_shared<DescriptorPool>(uniformBuffersObj);
        descriptorSetsObj = std::make_shared<DescriptorSets>(descriptorPoolObj);
        commandBufferObj = std::make_shared<CommandBuffers>(descriptorSetsObj);
        syncObjectsObj = std::make_shared<SyncObjects>(commandBufferObj);
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
        logicalDeviceObj->waitIdle();
    }

    void drawFrame() {
        syncObjectsObj->waitForFences(currentFrame, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(logicalDeviceObj->device, swapChainObj->swapChain, UINT64_MAX, syncObjectsObj->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        uniformBuffersObj->update(currentFrame);

        vkResetFences(logicalDeviceObj->device, 1, &syncObjectsObj->inFlightFences[currentFrame]);

        commandBufferObj->resetBuffer(currentFrame);
        commandBufferObj->recordBuffer(currentFrame, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { syncObjectsObj->imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBufferObj->commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { syncObjectsObj->renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(logicalDeviceObj->graphicsQueue, 1, &submitInfo, syncObjectsObj->inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChainObj->swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(logicalDeviceObj->presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void cleanup() {
        cleanupSwapChain();

        vkDestroyPipeline(logicalDeviceObj->device, graphicsPipelineObj->graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDeviceObj->device, graphicsPipelineObj->pipelineLayout, nullptr);
        vkDestroyRenderPass(logicalDeviceObj->device, renderPassObj->renderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(logicalDeviceObj->device, uniformBuffersObj->uniformBuffers[i], nullptr);
            vkFreeMemory(logicalDeviceObj->device, uniformBuffersObj->uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(logicalDeviceObj->device, descriptorPoolObj->descriptorPool, nullptr);

        vkDestroySampler(logicalDeviceObj->device, textureSamplerObj->textureSampler, nullptr);
        vkDestroyImageView(logicalDeviceObj->device, textureImageObj->textureImageView, nullptr);

        vkDestroyImage(logicalDeviceObj->device, textureImageObj->textureImage, nullptr);
        vkFreeMemory(logicalDeviceObj->device, textureImageObj->textureImageMemory, nullptr);

        vkDestroyDescriptorSetLayout(logicalDeviceObj->device, descriptorSetLayoutObj->descriptorSetLayout, nullptr);

        vkDestroyBuffer(logicalDeviceObj->device, indexBufferObj->indexBuffer, nullptr);
        vkFreeMemory(logicalDeviceObj->device, indexBufferObj->indexBufferMemory, nullptr);

        vkDestroyBuffer(logicalDeviceObj->device, vertexBufferObj->vertexBuffer, nullptr);
        vkFreeMemory(logicalDeviceObj->device, vertexBufferObj->vertexBufferMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(logicalDeviceObj->device, syncObjectsObj->renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDeviceObj->device, syncObjectsObj->imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(logicalDeviceObj->device, syncObjectsObj->inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(logicalDeviceObj->device, commandPoolObj->commandPool, nullptr);

        vkDestroyDevice(logicalDeviceObj->device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instanceObj->instance, debugMessengerObj->debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instanceObj->instance, surfaceObj->surface, nullptr);
        vkDestroyInstance(instanceObj->instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void cleanupSwapChain() {

        vkDestroyImageView(logicalDeviceObj->device, colorResourcesObj->colorImageView, nullptr);
        vkDestroyImage(logicalDeviceObj->device, colorResourcesObj->colorImage, nullptr);
        vkFreeMemory(logicalDeviceObj->device, colorResourcesObj->colorImageMemory, nullptr);

        vkDestroyImageView(logicalDeviceObj->device, depthResourcesObj->depthImageView, nullptr);
        vkDestroyImage(logicalDeviceObj->device, depthResourcesObj->depthImage, nullptr);
        vkFreeMemory(logicalDeviceObj->device, depthResourcesObj->depthImageMemory, nullptr);

        swapChainObj->destroyFramebuffers();
        imageViewsObj->destroyImageViews();
        swapChainObj->destroySwapChain();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        logicalDeviceObj->waitIdle();

        cleanupSwapChain();

        swapChainObj->create();
        imageViewsObj->create();
        colorResourcesObj->create();
        depthResourcesObj->create();
        frameBuffersObj->create();
    }
};
