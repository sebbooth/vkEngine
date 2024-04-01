#pragma once

#include <random>


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

const bool enableValidationLayers = true;

/*
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
*/



class ComputeApp {
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
    std::shared_ptr<Surface> p_Surface;
    std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
    std::shared_ptr<LogicalDevice> p_LogicalDevice;
    std::shared_ptr<SwapChain> p_SwapChain;
    std::shared_ptr<ImageViews> p_ImageViews;
    std::shared_ptr<RenderPass> p_RenderPass;
    std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout;
    std::shared_ptr<ComputeGraphicsPipeline> p_GraphicsPipeline;
    std::shared_ptr<CommandPool> p_CommandPool;
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

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<ComputeApp*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        p_Instance = std::make_shared<Instance>(enableValidationLayers, validationLayers, deviceExtensions);
        p_Instance->create();

        p_Surface = std::make_shared<Surface>(p_Instance, window);

        p_PhysicalDevice = std::make_shared<PhysicalDevice>(p_Surface);
        p_PhysicalDevice->msaaEnabled = false;
        p_PhysicalDevice->computeEnabled = true;
        p_PhysicalDevice->create();

        p_LogicalDevice = std::make_shared<LogicalDevice>(p_PhysicalDevice);
        p_LogicalDevice->wireFrameEnabled = false;
        p_LogicalDevice->create();

        p_SwapChain = std::make_shared<SwapChain>(p_LogicalDevice);
        p_SwapChain->create();

        p_ImageViews = std::make_shared<ImageViews>(p_SwapChain);
        p_ImageViews->create();

        p_RenderPass = std::make_shared<RenderPass>(p_ImageViews);
        p_RenderPass->depthEnabled = false;
        p_RenderPass->create();

        p_DescriptorSetLayout = std::make_shared<DescriptorSetLayout>(p_RenderPass);
        p_DescriptorSetLayout->guiEnabled = guiEnabled;
        p_DescriptorSetLayout->uboEnabled = true;
        p_DescriptorSetLayout->samplerEnabled = true;
        p_DescriptorSetLayout->create();

        createComputeDescriptorSetLayout();

        p_GraphicsPipeline = std::make_shared<ComputeGraphicsPipeline>(p_DescriptorSetLayout);
        p_GraphicsPipeline->cullBackFaces = false;
        p_GraphicsPipeline->displayNormals = false;
        p_GraphicsPipeline->create();


        createComputePipeline();


        p_CommandPool = std::make_shared<CommandPool>(p_GraphicsPipeline);
        p_CommandPool->create();

        createSSBOs();
        createComputeUniformBuffers();
        createComputeDescriptorPool();
        createComputeDescriptorSets();

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

        createComputeCommandBuffers();

        p_SyncObjects = std::make_shared<SyncObjects>(p_CommandBuffer);
        p_SyncObjects->create();


        p_Gui->init();
    }

    const int PARTICLE_COUNT = 256000;
    std::vector<VkBuffer> shaderStorageBuffers;
    std::vector<VkDeviceMemory> shaderStorageBuffersMemory;
    VkDescriptorSetLayout computeDescriptorSetLayout;
    VkPipelineLayout computePipelineLayout;
    VkPipeline computePipeline;
    VkDescriptorPool computeDescriptorPool;
    std::vector<VkDescriptorSet> computeDescriptorSets;
    std::vector<VkCommandBuffer> computeCommandBuffers;
    std::vector<VkBuffer> computeUniformBuffers;
    std::vector<VkDeviceMemory> computeUniformBuffersMemory;
    std::vector<void*> computeUniformBuffersMapped;
    float lastFrameTime = 0.0f;
    double lastTime = 0.0f;

    struct ComputeUniformBufferObject {
        float deltaTime = 1.0f;
    };

    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Particle);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Particle, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Particle, color);

            return attributeDescriptions;
        }
    };

    void createComputeDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 2;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(p_LogicalDevice->device, &poolInfo, nullptr, &computeDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createComputeDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, computeDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = computeDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        computeDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(p_LogicalDevice->device, &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo uniformBufferInfo{};
            uniformBufferInfo.buffer = computeUniformBuffers[i];
            uniformBufferInfo.offset = 0;
            uniformBufferInfo.range = sizeof(ComputeUniformBufferObject);

            std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = computeDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

            VkDescriptorBufferInfo storageBufferInfoLastFrame{};
            storageBufferInfoLastFrame.buffer = shaderStorageBuffers[(i - 1) % MAX_FRAMES_IN_FLIGHT];
            storageBufferInfoLastFrame.offset = 0;
            storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = computeDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

            VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
            storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers[i];
            storageBufferInfoCurrentFrame.offset = 0;
            storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = computeDescriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

            vkUpdateDescriptorSets(p_LogicalDevice->device, 3, descriptorWrites.data(), 0, nullptr);
        }
    }

    void createComputeDescriptorSetLayout() {
        std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
        layoutBindings[0].binding = 0;
        layoutBindings[0].descriptorCount = 1;
        layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBindings[0].pImmutableSamplers = nullptr;
        layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        layoutBindings[1].binding = 1;
        layoutBindings[1].descriptorCount = 1;
        layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBindings[1].pImmutableSamplers = nullptr;
        layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        layoutBindings[2].binding = 2;
        layoutBindings[2].descriptorCount = 1;
        layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBindings[2].pImmutableSamplers = nullptr;
        layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 3; 
        layoutInfo.pBindings = layoutBindings.data();

        if (vkCreateDescriptorSetLayout(p_LogicalDevice->device, &layoutInfo, nullptr, &computeDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute descriptor set layout!");
        }
    }

    void createSSBOs() {
        shaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        shaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

        // Initial particle positions on a circle
        std::vector<Particle> particles(PARTICLE_COUNT);
        for (auto& particle : particles) {
            float r = 0.25f * sqrt(rndDist(rndEngine));
            float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
            float x = r * cos(theta) * HEIGHT / WIDTH;
            float y = r * sin(theta);
            particle.position = glm::vec2(x, y);
            particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
            particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
        }

        VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

        // Create a staging buffer used to upload data to the gpu
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(p_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, particles.data(), (size_t)bufferSize);
        vkUnmapMemory(p_LogicalDevice->device, stagingBufferMemory);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderStorageBuffers[i], shaderStorageBuffersMemory[i]);
            // Copy data from the staging buffer (host) to the shader storage buffer (GPU)
            p_CommandPool->copyBuffer(stagingBuffer, shaderStorageBuffers[i], bufferSize);
        }
        vkDestroyBuffer(p_LogicalDevice->device, stagingBuffer, nullptr);
        vkFreeMemory(p_LogicalDevice->device, stagingBufferMemory, nullptr);
    }

    void createComputePipeline() {
        auto computeShaderCode = readFile("shaders/comp.spv");

        VkShaderModule computeShaderModule = p_GraphicsPipeline->createShaderModule(computeShaderCode);

        VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
        computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageInfo.module = computeShaderModule;
        computeShaderStageInfo.pName = "main";

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;

        if (vkCreatePipelineLayout(p_LogicalDevice->device, &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = computePipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(p_LogicalDevice->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(p_LogicalDevice->device, computeShaderModule, nullptr);
    }

    void createComputeUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        computeUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        computeUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        computeUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, computeUniformBuffers[i], computeUniformBuffersMemory[i]);

            vkMapMemory(p_LogicalDevice->device, computeUniformBuffersMemory[i], 0, bufferSize, 0, &computeUniformBuffersMapped[i]);
        }
    }

    void createComputeCommandBuffers() {
        computeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = p_CommandPool->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)computeCommandBuffers.size();

        if (vkAllocateCommandBuffers(p_LogicalDevice->device, &allocInfo, computeCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate compute command buffers!");
        }
    }

    void recordComputeCommandBuffer(VkCommandBuffer commandBuffer) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording compute command buffer!");
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSets[currentFrame], 0, nullptr);

        vkCmdDispatch(commandBuffer, PARTICLE_COUNT / 256, 1, 1);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record compute command buffer!");
        }
    }

    void updateComputeUniformBuffer(uint32_t currentImage) {
        ComputeUniformBufferObject ubo{};
        ubo.deltaTime = lastFrameTime * 2.0f;

        memcpy(computeUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
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
        p_DescriptorSetLayout->destroy();
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
        p_DescriptorSetLayout->guiEnabled = guiEnabled;
        p_DescriptorSetLayout->uboEnabled = true;
        p_DescriptorSetLayout->samplerEnabled = prevSamplerEnabled;
        p_DescriptorSetLayout->create();
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
            double currentTime = glfwGetTime();
            lastFrameTime = (currentTime - lastTime) * 1000.0;
            lastTime = currentTime;
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            fps = microsecPerSec / std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            std::cout << "\r" << "FPS: " << std::setw(10) << fps << std::flush;
        }
        std::cout << std::endl;
        p_LogicalDevice->waitIdle();
    }

    void drawFrame() {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Compute submission        
        vkWaitForFences(p_LogicalDevice->device, 1, &p_SyncObjects->computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        updateComputeUniformBuffer(currentFrame);

        vkResetFences(p_LogicalDevice->device, 1, &p_SyncObjects->computeInFlightFences[currentFrame]);

        vkResetCommandBuffer(computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        recordComputeCommandBuffer(computeCommandBuffers[currentFrame]);

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &computeCommandBuffers[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &p_SyncObjects->computeFinishedSemaphores[currentFrame];

        if (vkQueueSubmit(p_LogicalDevice->computeQueue, 1, &submitInfo, p_SyncObjects->computeInFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit compute command buffer!");
        };

        // Graphics submission
        vkWaitForFences(p_LogicalDevice->device, 1, &p_SyncObjects->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(p_LogicalDevice->device, p_SwapChain->swapChain, UINT64_MAX, p_SyncObjects->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(p_LogicalDevice->device, 1, &p_SyncObjects->inFlightFences[currentFrame]);

        vkResetCommandBuffer(p_CommandBuffer->commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        p_CommandBuffer->recordBuffer(
            p_CommandBuffer->commandBuffers[currentFrame], 
            currentFrame, 
            imageIndex, 
            shaderStorageBuffers, 
            PARTICLE_COUNT
        );
        
        VkSemaphore waitSemaphores[] = { p_SyncObjects->computeFinishedSemaphores[currentFrame], p_SyncObjects->imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = 2;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &p_CommandBuffer->commandBuffers[currentFrame];
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

        p_Gui->destroy();

        cleanupSwapChain();

        p_GraphicsPipeline->destroy();
        vkDestroyPipeline(p_LogicalDevice->device, computePipeline, nullptr);
        vkDestroyPipelineLayout(p_LogicalDevice->device, computePipelineLayout, nullptr);
        p_RenderPass->destroy();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(p_LogicalDevice->device, computeUniformBuffers[i], nullptr);
            vkFreeMemory(p_LogicalDevice->device, computeUniformBuffersMemory[i], nullptr);
        }
        vkDestroyDescriptorPool(p_LogicalDevice->device, computeDescriptorPool, nullptr);

        p_DescriptorPool->destroy();

        vkDestroyDescriptorSetLayout(p_LogicalDevice->device, computeDescriptorSetLayout, nullptr);
        p_DescriptorSetLayout->destroy();
        p_IndexBuffer->destroy();
        p_VertexBuffer->destroy();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(p_LogicalDevice->device, shaderStorageBuffers[i], nullptr);
            vkFreeMemory(p_LogicalDevice->device, shaderStorageBuffersMemory[i], nullptr);
        }
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
