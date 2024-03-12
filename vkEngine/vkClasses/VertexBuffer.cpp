#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(std::shared_ptr<Model> modelObj)
{
    this->modelObj = modelObj;
    std::shared_ptr<LogicalDevice> logicalDeviceObj = modelObj->
        textureSamplerObj->
        textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        colorResourcesObj->
        commandPoolObj->
        graphicsPipelineObj->
        descriptorSetLayoutObj->
        renderPassObj->
        imageViewsObj->
        swapChainObj->
        logicalDeviceObj;

    VkDeviceSize bufferSize = sizeof(modelObj->vertices[0]) * modelObj->vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    logicalDeviceObj->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDeviceObj->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, modelObj->vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDeviceObj->device, stagingBufferMemory);

    logicalDeviceObj->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    modelObj->
        textureSamplerObj->
        textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        colorResourcesObj->
        commandPoolObj->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDeviceObj->device, stagingBuffer, nullptr);
    vkFreeMemory(logicalDeviceObj->device, stagingBufferMemory, nullptr);
}
