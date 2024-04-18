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

struct ComputeUniformBufferObject {
    alignas(16) glm::vec3 camPos = glm::vec3(262.905243, 270.311707, 282.328888);
    alignas(16) glm::vec3 camDir = glm::vec3(-0.519135, -0.634374, -0.572781);
    alignas(16) glm::vec3 camUp = glm::vec3(-0.422386, 0.773021, -0.473317);
    alignas(16) glm::vec3 sunDir = glm::vec3(-1, -2, -3);

    float deltaTime = 1.0f;
    int width = 600;
    int height = 800;
    int octreeSize;
    int octreeMaxDepth;
};

class TestRewrite {

public:
    void run() {
        initSettings();
        createOctree();
        initWindow();
        initVulkan();
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

    std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout;
    std::shared_ptr<ComputeDescriptorSetLayout> p_ComputeDescriptorSetLayout;
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

    std::shared_ptr<DescriptorPool> p_ComputeDescriptorPool;
    std::shared_ptr<DescriptorSets> p_ComputeDescriptorSets;

    std::shared_ptr<DepthResources> p_DepthResources;
    std::shared_ptr<ColorResources> p_ColorResources;

    std::shared_ptr<FrameBuffers> p_FrameBuffers;

    void initSettings() {
        config = std::make_shared<VkConfig>();
        config->enableValidationLayers = enableValidationLayers;
        config->deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        config->validationLayers = { "VK_LAYER_KHRONOS_validation" };
        config->computeEnabled = true;
        config->msaaEnabled = true;
        config->depthStencilEnabled = true;
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(config->WIDTH, config->HEIGHT, "VkEngine", nullptr, nullptr);
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

        p_CommandPool = std::make_shared<CommandPool>(
            p_PhysicalDevice->queueFamilies,
            p_LogicalDevice->device,
            p_LogicalDevice->graphicsQueue
        );
        p_CommandPool->create();

        initShaderResources();
        initComputeDescriptorSets();
        initRenderAttachments();

        p_FrameBuffers = std::make_shared<FrameBuffers>(
            p_LogicalDevice->device,
            p_ImageViews->swapChainImageViews,
            p_SwapChain->swapChainExtent,
            p_RenderPass->renderPass,
            config
        );
        p_FrameBuffers->bindSwapChainImageViews(colorPassBinding);
        p_FrameBuffers->bindDepthResources(depthPassBinding, p_DepthResources->depthImageView);
        p_FrameBuffers->bindColorReources(colorResolveBinding, p_ColorResources->colorImageView);
        p_FrameBuffers->create();

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
        depthPassBinding = p_RenderPass->bindDepthPass();
        colorResolveBinding = p_RenderPass->bindMsaaPass();
        p_RenderPass->create();
    }

    void initDescriptorSetLayouts() {
        /*
            TODO:
                1. Create parent "DescriptorSetLayout" class.
                2. Allow for similar user control in graphics DSL as what is now in compute DSL.
        */
        p_DescriptorSetLayout = std::make_shared<DescriptorSetLayout>(
            p_LogicalDevice->device,
            config
        );
        p_DescriptorSetLayout->create();

        p_ComputeDescriptorSetLayout = std::make_shared<ComputeDescriptorSetLayout>(p_LogicalDevice->device);

        // Note: Must do DSL bindings in order of bindings in shader code
        // Should be fine to do bindings in any order for rest of code (Descriptor Pools and Sets)
        // since binding functions will be called with the binding indices returned here
        computeUniformBinding = p_ComputeDescriptorSetLayout->bindUniformBuffer();  //0
        computeSSBOBinding = p_ComputeDescriptorSetLayout->bindStorageBuffer();     //1
        computeImageBinding = p_ComputeDescriptorSetLayout->bindStorageImage();     //2
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
            p_DescriptorSetLayout->descriptorSetLayout,
            config
        );
        std::string vertexShaderFile = "shaders/vertNoUBO.spv";
        std::string fragmentShaderFile = "shaders/frag.spv";
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
    }

    void initComputeDescriptorSets() {
        p_ComputeDescriptorPool = std::make_shared<DescriptorPool>(
            p_LogicalDevice->device,
            config
        );
        p_ComputeDescriptorPool->bindUniformBuffer(computeUniformBinding, config->MAX_FRAMES_IN_FLIGHT);
        p_ComputeDescriptorPool->bindStorageBuffer(computeSSBOBinding, config->MAX_FRAMES_IN_FLIGHT);
        p_ComputeDescriptorPool->bindStorageImage(computeImageBinding, config->MAX_FRAMES_IN_FLIGHT);
        p_ComputeDescriptorPool->create();

        p_ComputeDescriptorSets = std::make_shared<DescriptorSets>(
            p_LogicalDevice->device,
            p_ComputeDescriptorSetLayout->descriptorSetLayout,
            p_ComputeDescriptorPool->descriptorPool,
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

    void cleanup() {
        p_FrameBuffers->destroy();

        p_ColorResources->destroy();
        p_DepthResources->destroy();

        p_ComputeDescriptorPool->destroy();

        p_ComputeUBO->destroy();
        p_CanvasSampler->destroy();
        p_CanvasImage->destroy();
        p_OctreeSSBO->destroy();

        p_CommandPool->destroy();

        p_ComputePipeline->destroy();
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

    Octree octree;
    int octreeDepth = 5;
    int octreeWidth = static_cast<int>(pow(2, octreeDepth));
    bool visualizeOctree = false;
    bool terrain = false;
    bool shadows = false;

    void createOctree() {
        int depth = octreeDepth;
        int width = octreeWidth;

        Perlin perlinGenerator;
        perlinGenerator.layers = octreeDepth - 1;
        perlinGenerator.init_amp = 2;
        perlinGenerator.grid_size = width * 2;

        SimplexNoise simplexNoiseGenerator = SimplexNoise(8.0f, 3.0f, 3.0f, 0.1f);

        std::random_device rd;  // Seed for random number engine
        std::mt19937 gen(rd()); // Mersenne Twister random number engine
        std::uniform_int_distribution<> distribution(-octreeWidth / 20, octreeWidth / 20); // Range from 1 to 100
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
