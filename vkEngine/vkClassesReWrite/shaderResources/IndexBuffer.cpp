#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(
    std::shared_ptr<LogicalDevice> p_LogicalDevice,
    std::shared_ptr<CommandPool> p_CommandPool
)
{
    mp_LogicalDevice = p_LogicalDevice;
    mp_CommandPool = p_CommandPool;
}

void IndexBuffer::create(std::vector<uint32_t> indices)
{
    this->indices = indices;

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer{};
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
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(mp_LogicalDevice->device, stagingBufferMemory);

    mp_LogicalDevice->createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        indexBuffer, 
        indexBufferMemory
    );
    
    mp_CommandPool->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(mp_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(mp_LogicalDevice->device, stagingBufferMemory, nullptr);
}

void IndexBuffer::destroy()
{
    vkDestroyBuffer(mp_LogicalDevice->device, indexBuffer, nullptr);
    vkFreeMemory(mp_LogicalDevice->device, indexBufferMemory, nullptr);
}
