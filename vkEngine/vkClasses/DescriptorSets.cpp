#include "DescriptorSets.h"

DescriptorSets::DescriptorSets(std::shared_ptr<DescriptorPool> p_DescriptorPool)
{
    this->p_DescriptorPool = p_DescriptorPool;
    this->p_FrameBuffers = p_DescriptorPool->p_FrameBuffers;
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

void DescriptorSets::create()
{
    std::vector<VkDescriptorSetLayout> layouts(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT, p_DescriptorSetLayout->descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = p_DescriptorPool->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(p_LogicalDevice->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < p_FrameBuffers->MAX_FRAMES_IN_FLIGHT; i++) {

        std::vector<VkWriteDescriptorSet> descriptorWrites{};

        VkDescriptorBufferInfo uboBufferInfo{};
        VkWriteDescriptorSet uboDescriptorWrite{};

        if (p_DescriptorSetLayout->uboEnabled) {
            uboBufferInfo.buffer = p_DescriptorPool->p_UniformBuffers->uniformBuffers[i];
            uboBufferInfo.offset = 0;
            uboBufferInfo.range = sizeof(UniformBufferObject);

            uboDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uboDescriptorWrite.dstSet = descriptorSets[i];
            uboDescriptorWrite.dstBinding = 0;
            uboDescriptorWrite.dstArrayElement = 0;
            uboDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboDescriptorWrite.descriptorCount = 1;
            uboDescriptorWrite.pBufferInfo = &uboBufferInfo;
            
            descriptorWrites.push_back(uboDescriptorWrite);
        }
        
        VkDescriptorImageInfo samplerImageInfo{};
        VkWriteDescriptorSet samplerDescriptorWrite{};

        if (p_DescriptorSetLayout->samplerEnabled) {
            samplerImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            samplerImageInfo.imageView = p_DescriptorPool->p_TextureSampler->p_TextureImage->textureImageView;
            samplerImageInfo.sampler = p_DescriptorPool->p_TextureSampler->textureSampler;

            samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerDescriptorWrite.dstSet = descriptorSets[i];
            samplerDescriptorWrite.dstBinding = 1;
            samplerDescriptorWrite.dstArrayElement = 0;
            samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerDescriptorWrite.descriptorCount = 1;
            samplerDescriptorWrite.pImageInfo = &samplerImageInfo;

            descriptorWrites.push_back(samplerDescriptorWrite);
        }

        vkUpdateDescriptorSets(p_LogicalDevice->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void DescriptorSets::createWithStorageSampler(std::vector<VkImageView> imageViews, std::vector<VkSampler> imageSamplers)
{
    std::vector<VkDescriptorSetLayout> layouts(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT, p_DescriptorSetLayout->descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = p_DescriptorPool->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(p_LogicalDevice->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < p_FrameBuffers->MAX_FRAMES_IN_FLIGHT; i++) {

        std::vector<VkWriteDescriptorSet> descriptorWrites{};

        VkDescriptorBufferInfo uboBufferInfo{};
        VkWriteDescriptorSet uboDescriptorWrite{};

        if (p_DescriptorSetLayout->uboEnabled) {
            uboBufferInfo.buffer = p_DescriptorPool->p_UniformBuffers->uniformBuffers[i];
            uboBufferInfo.offset = 0;
            uboBufferInfo.range = sizeof(UniformBufferObject);

            uboDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uboDescriptorWrite.dstSet = descriptorSets[i];
            uboDescriptorWrite.dstBinding = 0;
            uboDescriptorWrite.dstArrayElement = 0;
            uboDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboDescriptorWrite.descriptorCount = 1;
            uboDescriptorWrite.pBufferInfo = &uboBufferInfo;

            descriptorWrites.push_back(uboDescriptorWrite);
        }

        VkDescriptorImageInfo samplerImageInfo{};
        VkWriteDescriptorSet samplerDescriptorWrite{};

        if (p_DescriptorSetLayout->samplerEnabled) {
            samplerImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            samplerImageInfo.imageView = imageViews[i];
            samplerImageInfo.sampler = imageSamplers[i];

            samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerDescriptorWrite.dstSet = descriptorSets[i];
            samplerDescriptorWrite.dstBinding = 1;
            samplerDescriptorWrite.dstArrayElement = 0;
            samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerDescriptorWrite.descriptorCount = 1;
            samplerDescriptorWrite.pImageInfo = &samplerImageInfo;

            descriptorWrites.push_back(samplerDescriptorWrite);
        }

        vkUpdateDescriptorSets(p_LogicalDevice->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}


void DescriptorSets::destroy()
{
    if (p_DescriptorSetLayout->uboEnabled) {

    }

    if (p_DescriptorSetLayout->samplerEnabled) {

    }
}
