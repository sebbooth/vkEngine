#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(
    std::shared_ptr<LogicalDevice> p_LogicalDevice,
    std::shared_ptr<CommandPool> p_CommandPool
)
{
    mp_LogicalDevice = p_LogicalDevice;
    mp_CommandPool = p_CommandPool;
}

void VertexBuffer::create(std::vector<Vertex> vertices)
{
    this->vertices = vertices;

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mp_LogicalDevice->createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(mp_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(mp_LogicalDevice->device, stagingBufferMemory);

    mp_LogicalDevice->createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        vertexBuffer, 
        vertexBufferMemory
    );

    mp_CommandPool->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(mp_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(mp_LogicalDevice->device, stagingBufferMemory, nullptr);
}

void VertexBuffer::destroy()
{
    vkDestroyBuffer(mp_LogicalDevice->device, vertexBuffer, nullptr);
    vkFreeMemory(mp_LogicalDevice->device, vertexBufferMemory, nullptr);
}
