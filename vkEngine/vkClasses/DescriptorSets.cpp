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

        if (p_DescriptorSetLayout->uboEnabled) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = p_DescriptorPool->p_UniformBuffers->uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            
            descriptorWrites.push_back(descriptorWrite);
        }
        
        if (p_DescriptorSetLayout->samplerEnabled) {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = p_DescriptorPool->p_TextureSampler->p_TextureImage->textureImageView;
            imageInfo.sampler = p_DescriptorPool->p_TextureSampler->textureSampler;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 1;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            descriptorWrites.push_back(descriptorWrite);
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
