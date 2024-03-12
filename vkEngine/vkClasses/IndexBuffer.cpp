#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::shared_ptr<VertexBuffer> vertexBufferObj)
{
    this->vertexBufferObj = vertexBufferObj;

    VkDeviceSize bufferSize = sizeof(vertexBufferObj->modelObj->indices[0]) * vertexBufferObj->modelObj->indices.size();
    
    std::shared_ptr<LogicalDevice> logicalDeviceObj = vertexBufferObj->
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

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory;
    logicalDeviceObj->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDeviceObj->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertexBufferObj->modelObj->indices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDeviceObj->device, stagingBufferMemory);

    logicalDeviceObj->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    vertexBufferObj->
        modelObj->
        textureSamplerObj->
        textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        commandPoolObj->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(logicalDeviceObj->device, stagingBuffer, nullptr);
    vkFreeMemory(logicalDeviceObj->device, stagingBufferMemory, nullptr);
}
