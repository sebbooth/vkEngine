#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(std::shared_ptr<FrameBuffers> p_FrameBuffers)
{
    this->p_FrameBuffers = p_FrameBuffers;
    this->p_CommandPool = p_FrameBuffers->p_CommandPool;
    this->p_GraphicsPipeline = p_CommandPool->p_GraphicsPipeline;
    this->p_DescriptorSetLayout = p_GraphicsPipeline->p_DescriptorSetLayout;
    this->p_RenderPass = p_DescriptorSetLayout->p_RenderPass;
    this->p_ImageViews = p_RenderPass->p_ImageViews;
    this->p_SwapChain = p_ImageViews->p_SwapChain;
    this->p_LogicalDevice = p_SwapChain->p_LogicalDevice;
    this->p_PhysicalDevice = p_LogicalDevice->p_PhysicalDevice;
    this->p_Surface = p_PhysicalDevice->p_Surface;
    this->p_Instance = p_Surface->p_Instance;
}

void DescriptorPool::create()
{

    uint32_t maxSets = 0;

    VkDescriptorPoolSize uboPoolSize{};

    if (p_DescriptorSetLayout->uboEnabled) {
        this->p_UniformBuffers = std::make_shared<UniformBuffers>(p_FrameBuffers);
        p_UniformBuffers->create();

        uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboPoolSize.descriptorCount = static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);

        poolSizes.push_back(uboPoolSize);
    }

    VkDescriptorPoolSize texturePoolSize{};

    if (true) {
        if (!p_GraphicsPipeline->simpleShader) {
            this->p_TextureImage = std::make_shared<TextureImage>(p_FrameBuffers);
            p_TextureImage->mipmapsEnabled = true;
            p_TextureImage->create();

            this->p_TextureSampler = std::make_shared<TextureSampler>(p_TextureImage);
            p_TextureSampler->create();
        }
        
        texturePoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texturePoolSize.descriptorCount = static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);

        maxSets += static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);

        poolSizes.push_back(texturePoolSize);
    }

    VkDescriptorPoolSize guiPoolSize{};

    guiPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    guiPoolSize.descriptorCount = static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    maxSets += static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);

    poolSizes.push_back(guiPoolSize);


    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
   
    // gui
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;

    if (vkCreateDescriptorPool(p_LogicalDevice->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorPool::update(uint32_t currentFrame)
{
    if (p_DescriptorSetLayout->uboEnabled) {
        p_UniformBuffers->update(currentFrame);
    }
}

void DescriptorPool::destroy()
{
    if (p_DescriptorSetLayout->uboEnabled) {
        for (size_t i = 0; i < p_FrameBuffers->MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(p_LogicalDevice->device, p_UniformBuffers->uniformBuffers[i], nullptr);
            vkFreeMemory(p_LogicalDevice->device, p_UniformBuffers->uniformBuffersMemory[i], nullptr);
        }
    }

    vkDestroyDescriptorPool(p_LogicalDevice->device, descriptorPool, nullptr);

    if ((p_DescriptorSetLayout->samplerEnabled || p_DescriptorSetLayout->guiEnabled) && !p_GraphicsPipeline->simpleShader) {
        vkDestroySampler(p_LogicalDevice->device, p_TextureSampler->textureSampler, nullptr);
        vkDestroyImageView(p_LogicalDevice->device, p_TextureImage->textureImageView, nullptr);

        vkDestroyImage(p_LogicalDevice->device, p_TextureImage->textureImage, nullptr);
        vkFreeMemory(p_LogicalDevice->device, p_TextureImage->textureImageMemory, nullptr);
    }
}
