#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(std::shared_ptr<UniformBuffers> uniformBuffersObj)
{
    this->uniformBuffersObj = uniformBuffersObj;

    std::shared_ptr<LogicalDevice> logicalDeviceObj = uniformBuffersObj->
                                                        indexBufferObj->
                                                        vertexBufferObj->
                                                        modelObj->
                                                        textureSamplerObj->
                                                        textureImageObj->
                                                        frameBuffersObj->
                                                        depthResourcesObj->
                                                        commandPoolObj->
                                                        graphicsPipelineObj->
                                                        descriptorSetLayoutObj->
                                                        renderPassObj->
                                                        imageViewsObj->
                                                        swapChainObj->
                                                        logicalDeviceObj;


    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(logicalDeviceObj->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}
