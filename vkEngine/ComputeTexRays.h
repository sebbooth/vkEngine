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
#include "glm/ext.hpp"

#define STB_IMAGE_IMPLEMENTATION

#define TINYOBJLOADER_IMPLEMENTATION

#include <iomanip> // For std::setw

#include "VkClasses.h"
#include "Octree.h"
#include "Perlin.h"
#include "SimplexNoise.h"

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



class ComputeTexRays {
public:
    void run() {
        createOctree();
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
    std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
    std::shared_ptr<CommandPool> p_CommandPool;
    std::shared_ptr<FrameBuffers> p_FrameBuffers;
    std::shared_ptr<UniformBuffers> p_UniformBuffers;
    std::shared_ptr<DescriptorPool> p_DescriptorPool;
    std::shared_ptr<DescriptorSets> p_DescriptorSets;
    std::shared_ptr<CommandBuffers> p_CommandBuffer;
    std::shared_ptr<SyncObjects> p_SyncObjects;


    std::shared_ptr<VertexBuffer> p_VertexBuffer;
    std::shared_ptr<IndexBuffer> p_IndexBuffer;

    bool guiEnabled = true;

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<ComputeTexRays*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    const std::vector<Vertex> vertices = {
        {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    std::vector<uint32_t> indices = {
        0,1,2,
        2,3,0
    };

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

        p_GraphicsPipeline = std::make_shared<GraphicsPipeline>(p_DescriptorSetLayout);

        p_GraphicsPipeline->cullBackFaces = false;
        p_GraphicsPipeline->displayNormals = false;
        p_GraphicsPipeline->simpleShader = true;
        p_GraphicsPipeline->create();

        createComputePipeline();

        p_CommandPool = std::make_shared<CommandPool>(p_GraphicsPipeline);
        p_CommandPool->create();

        createSSBOs();

        createStorageImages();
        createSamplers();

        createComputeUniformBuffers();
        createComputeDescriptorPool();
        createComputeDescriptorSets();

        p_FrameBuffers = std::make_shared<FrameBuffers>(p_CommandPool);
        p_FrameBuffers->create();

        p_VertexBuffer = std::make_shared<VertexBuffer>(p_FrameBuffers);
        p_VertexBuffer->create(vertices);
        p_IndexBuffer = std::make_shared<IndexBuffer>(p_VertexBuffer);
        p_IndexBuffer->create(indices);

        p_DescriptorPool = std::make_shared<DescriptorPool>(p_FrameBuffers);
        p_DescriptorPool->create();
        p_DescriptorSets = std::make_shared<DescriptorSets>(p_DescriptorPool);
        p_DescriptorSets->createWithStorageSampler(storageImageViews, storageImageSamplers);
        p_CommandBuffer = std::make_shared<CommandBuffers>(p_DescriptorSets);

        p_CommandBuffer->attachVertexBuffer(p_VertexBuffer);
        p_CommandBuffer->attachIndexBuffer(p_IndexBuffer);
        p_CommandBuffer->create();

        createComputeCommandBuffers();

        p_SyncObjects = std::make_shared<SyncObjects>(p_CommandBuffer);
        p_SyncObjects->create();
    }

    const int PARTICLE_COUNT = 800 * 600;
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
        /*
        alignas(16) glm::vec3 camPos = glm::vec3(32, 32, -14);
        alignas(16) glm::vec3 camDir = glm::vec3(0, 0, 1);
        alignas(16) glm::vec3 camUp = glm::vec3(0, 1, 0);
        */

        alignas(16) glm::vec3 camPos = glm::vec3(577.516724, 565.847046, 484.979584);
        alignas(16) glm::vec3 camDir = glm::vec3(-0.647795, -0.570642, -0.504715);
        alignas(16) glm::vec3 camUp = glm::vec3(-0.557979, 0.806464, -0.195643);
        /*
        Cam Pos: vec3(577.516724, 565.847046, 484.979584)
Cam Dir: vec3(-0.647795, -0.570642, -0.504715)
Cam Up: vec3(-0.557979, 0.806464, -0.195643)
*/


        alignas(16) glm::vec3 sunDir = glm::vec3(-1,-2,-3);

        float deltaTime = 1.0f;
        int width = 600;
        int height = 800;
        int octreeSize;
        int octreeMaxDepth;
    };

    void createComputeDescriptorPool() {
        std::array<VkDescriptorPoolSize, 3> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 3;
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

            std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
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
            storageBufferInfoLastFrame.range = sizeof(OctreeNode) * octree.octreeArray.size();

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = computeDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

            VkDescriptorImageInfo storageImageInfo{};
            storageImageInfo.imageView = storageImageViews[i];
            storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            
            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = computeDescriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pImageInfo = &storageImageInfo;
         
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
        layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
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

    std::vector<VkImage> storageImages;
    std::vector<VkDeviceMemory> storageImageMemories;
    std::vector<VkImageView> storageImageViews;
    std::vector<VkSampler> storageImageSamplers;
    Octree octree;
    int octreeDepth = 7;
    int octreeWidth = pow(2, octreeDepth);
    bool visualizeOctree = false;
    bool terrain = true;
    bool shadows = true;
    void createOctree() {

        int depth = octreeDepth;
        int width = octreeWidth;

        Perlin perlinGenerator;
        perlinGenerator.layers = octreeDepth-1;
        perlinGenerator.init_amp = 2;
        perlinGenerator.grid_size = width * 2;

        SimplexNoise simplexNoiseGenerator = SimplexNoise(8.0f, 3.0f, 3.0f, 0.1f);



        std::random_device rd;  // Seed for random number engine
        std::mt19937 gen(rd()); // Mersenne Twister random number engine
        std::uniform_int_distribution<> distribution(-.05 * octreeWidth, .05 * octreeWidth); // Range from 1 to 100
        std::uniform_int_distribution<> materialDist(0, 5); // Range from 1 to 100

        std::vector<Voxel> testRandomVoxels;

        std::cout << "Generating Random Voxels... " << std::endl;


        for (int x = 0; x < width; x++) {
            for (int y = 0; y < width; y++) {
                for (int z = 0; z < width; z++) {
                    if (terrain) {
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
                            voxel.mat = -2 - materialDist(gen);   // -2 - -7 grass
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

    void createStorageImages() {
        storageImages.resize(MAX_FRAMES_IN_FLIGHT);
        storageImageMemories.resize(MAX_FRAMES_IN_FLIGHT);
        storageImageViews.resize(MAX_FRAMES_IN_FLIGHT);
        storageImageSamplers.resize(MAX_FRAMES_IN_FLIGHT);

        uint32_t width = p_SwapChain->swapChainExtent.width;
        uint32_t height = p_SwapChain->swapChainExtent.height;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            p_LogicalDevice->createImage(
                width,
                height,
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                storageImages[i],
                storageImageMemories[i],
                1,
                VK_SAMPLE_COUNT_1_BIT
            );

            transitionImageLayout(
                storageImages[i], 
                VK_FORMAT_R8G8B8A8_UNORM, 
                VK_IMAGE_LAYOUT_UNDEFINED, 
                VK_IMAGE_LAYOUT_GENERAL
            );

            storageImageViews[i] = p_ImageViews->createImageView(
                storageImages[i],
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1
            );

            //createTexSampler(storageImageSamplers[i]);
        }
    }

    void destroyStorageImages() {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySampler(p_LogicalDevice->device, storageImageSamplers[i], nullptr);
            vkDestroyImageView(p_LogicalDevice->device, storageImageViews[i], nullptr);
            vkDestroyImage(p_LogicalDevice->device, storageImages[i], nullptr);
            vkFreeMemory(p_LogicalDevice->device, storageImageMemories[i], nullptr);
        }
    }

    void createTexSampler(VkSampler textureSampler) {
        VkPhysicalDeviceProperties  properties{};
        vkGetPhysicalDeviceProperties(p_PhysicalDevice->physicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(1);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(p_LogicalDevice->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = p_CommandPool->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        p_CommandPool->endSingleTimeCommands(commandBuffer);
    }

    void createSamplers() {
        VkPhysicalDeviceProperties  properties{};
        vkGetPhysicalDeviceProperties(p_PhysicalDevice->physicalDevice, &properties);

        storageImageSamplers.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(1);
            samplerInfo.mipLodBias = 0.0f;

            if (vkCreateSampler(p_LogicalDevice->device, &samplerInfo, nullptr, &storageImageSamplers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture sampler!");
            }
        }
    }

    void createSSBOs() {
        shaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        shaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

        VkDeviceSize bufferSize = sizeof(OctreeNode) * octree.octreeArray.size();

        // Create a staging buffer used to upload data to the gpu
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(p_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, octree.octreeArray.data(), (size_t)bufferSize);
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
        auto computeShaderCode = readFile("shaders/raycast_comp.spv");
        if (shadows) computeShaderCode = readFile("shaders/shadowRays.spv");
        if (visualizeOctree) computeShaderCode = readFile("shaders/raycast_vis_comp.spv");

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
        VkDeviceSize bufferSize = sizeof(ComputeUniformBufferObject);

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

        vkCmdDispatch(commandBuffer, std::ceil(p_SwapChain->swapChainExtent.width * p_SwapChain->swapChainExtent.height / 256), 1, 1);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record compute command buffer!");
        }
    }

    ComputeUniformBufferObject ubo{};
    float rotateSpeed = 0.01;
    float moveSpeed = 0.05;
    bool p_Clicked = false;
    
    double cursorX = -999;
    double cursorY = -999;
    double prevCursorX = cursorX;
    double prevCursorY = cursorY;

    void updateComputeUniformBuffer(uint32_t currentImage) {
        ubo.octreeSize = octreeWidth;
        ubo.octreeMaxDepth = octreeDepth;
        ubo.deltaTime = lastFrameTime * 2.0f;

        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, 0.0001f * ubo.deltaTime, glm::vec3(0,1,0));
        ubo.sunDir = glm::vec3(rotationMat * glm::vec4(ubo.sunDir, 1.0));

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec3 camLeft = glm::cross(ubo.camUp, ubo.camDir);
            ubo.camPos += ubo.deltaTime * moveSpeed * camLeft;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::vec3 camLeft = glm::cross(ubo.camUp, ubo.camDir);
            ubo.camPos -= ubo.deltaTime * moveSpeed * camLeft;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            ubo.camPos += ubo.deltaTime * moveSpeed * ubo.camDir;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            ubo.camPos -= ubo.deltaTime * moveSpeed * ubo.camDir;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            ubo.camPos += ubo.deltaTime * moveSpeed * ubo.camUp;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            ubo.camPos -= ubo.deltaTime * moveSpeed * ubo.camUp;

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            p_Clicked = true;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && p_Clicked) {
            std::cout << "\nCam Pos: " << glm::to_string(ubo.camPos);
            std::cout << "\nCam Dir: " << glm::to_string(ubo.camDir);
            std::cout << "\nCam Up: " << glm::to_string(ubo.camUp) << "\n";
            p_Clicked = false;
        }



        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            float deltaX = 0;
            float deltaY = 0;

            glfwGetCursorPos(window, &cursorX, &cursorY);
            if (!(prevCursorX == -999 && prevCursorY == -999)) {
                deltaX = prevCursorX - cursorX;
                deltaY = prevCursorY - cursorY;
            }
            prevCursorX = cursorX;
            prevCursorY = cursorY;

            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, rotateSpeed * deltaX, ubo.camUp);
            ubo.camDir = glm::vec3(rotationMat * glm::vec4(ubo.camDir, 1.0));

            rotationMat = glm::mat4(1);
            glm::vec3 camLeft = glm::cross(ubo.camUp, ubo.camDir);
            rotationMat = glm::rotate(rotationMat, -rotateSpeed * deltaY, camLeft);
            ubo.camDir = glm::vec3(rotationMat * glm::vec4(ubo.camDir, 1.0));
            ubo.camUp = glm::vec3(rotationMat * glm::vec4(ubo.camUp, 1.0));
        }
        else {
            prevCursorX = -999;
            prevCursorY = -999;
        }
        
        ubo.height = p_SwapChain->swapChainExtent.height;
        ubo.width = p_SwapChain->swapChainExtent.width;
        memcpy(computeUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
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
        /*
            vkResetCommandBuffer(p_CommandBuffer->commandBuffers[currentFrame], 0);
            p_CommandBuffer->recordBuffer(
                p_CommandBuffer->commandBuffers[currentFrame],
                currentFrame,
                imageIndex,
                shaderStorageBuffers,
                PARTICLE_COUNT
            );
        */
        transitionImageLayout(storageImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        p_DescriptorPool->update(currentFrame);
        p_CommandBuffer->recordBuffer(currentFrame, imageIndex);

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
        transitionImageLayout(storageImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void cleanup() {

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

            vkDestroySampler(p_LogicalDevice->device, storageImageSamplers[i], nullptr);
            vkDestroyImageView(p_LogicalDevice->device, storageImageViews[i], nullptr);
            vkDestroyImage(p_LogicalDevice->device, storageImages[i], nullptr);
            vkFreeMemory(p_LogicalDevice->device, storageImageMemories[i], nullptr);
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
        

        vkDestroyDescriptorPool(p_LogicalDevice->device, computeDescriptorPool, nullptr);

        p_DescriptorPool->destroy();
        //vkDestroyDescriptorSetLayout(p_LogicalDevice->device, computeDescriptorSetLayout, nullptr);
        //p_DescriptorSetLayout->destroy();

        destroyStorageImages();

        p_SwapChain->create();
        p_ImageViews->create();
        p_FrameBuffers->create();

        createStorageImages();
        createSamplers();

        createComputeDescriptorPool();
        createComputeDescriptorSets();


        p_DescriptorPool->create();
        p_DescriptorSets->createWithStorageSampler(storageImageViews, storageImageSamplers);

      
        p_CommandBuffer->create();
        transitionImageLayout(storageImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
};
