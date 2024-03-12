#include "DescriptorSets.h"

DescriptorSets::DescriptorSets(std::shared_ptr<DescriptorPool> descriptorPoolObj)
{
    this->descriptorPoolObj = descriptorPoolObj;

    std::shared_ptr<TextureSampler> textureSamplerObj = descriptorPoolObj->
        uniformBuffersObj->
        indexBufferObj->
        vertexBufferObj->
        modelObj->
        textureSamplerObj;

    std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutObj = textureSamplerObj->
        textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        colorResourcesObj->
        commandPoolObj->
        graphicsPipelineObj->
        descriptorSetLayoutObj;

    std::shared_ptr<LogicalDevice> logicalDeviceObj = descriptorSetLayoutObj->
        renderPassObj->
        imageViewsObj->
        swapChainObj->
        logicalDeviceObj;

    std::vector<VkDescriptorSetLayout> layouts(descriptorPoolObj->uniformBuffersObj->MAX_FRAMES_IN_FLIGHT, descriptorSetLayoutObj->descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPoolObj->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorPoolObj->uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(descriptorPoolObj->uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(logicalDeviceObj->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < descriptorPoolObj->uniformBuffersObj->MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = descriptorPoolObj->uniformBuffersObj->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureSamplerObj->textureImageObj->textureImageView;
        imageInfo.sampler = textureSamplerObj->textureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(logicalDeviceObj->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
