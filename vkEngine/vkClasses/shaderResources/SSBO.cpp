#include "SSBO.h"

SSBO::SSBO(
    std::shared_ptr<LogicalDevice> logicalDevice,
    std::shared_ptr<CommandPool> commandPool,
    std::shared_ptr<VkConfig> config
)
{
	m_LogicalDevice = logicalDevice;
    m_CommandPool = commandPool;
	m_Config = config;
}

void SSBO::uploadData(VkDeviceSize bufferSize, const void* uploadData)
{
    
    m_BufferSize = bufferSize;
    

    // Create a staging buffer used to upload data to the gpu

    m_LogicalDevice->createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    vkMapMemory(m_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &stagingBufferMapped);
    memcpy(stagingBufferMapped, uploadData, (size_t)bufferSize);
    //vkUnmapMemory(m_LogicalDevice->device, stagingBufferMemory);

    m_LogicalDevice->createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        shaderStorageBuffer, 
        shaderStorageBufferMemory
    );
    // Copy data from the staging buffer (host) to the shader storage buffer (GPU)
    m_CommandPool->copyBuffer(stagingBuffer, shaderStorageBuffer, bufferSize);
    
    
    
    /*
    m_BufferSize = bufferSize;

    m_LogicalDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        shaderStorageBuffer,
        shaderStorageBufferMemory
    );
    vkMapMemory(m_LogicalDevice->device, shaderStorageBufferMemory, 0, bufferSize, 0, &shaderStorageBufferMemoryMapped);
    memcpy(shaderStorageBufferMemoryMapped, uploadData, (size_t)m_BufferSize);
    */

}

void SSBO::uploadData1(VkDeviceSize bufferSize, const void* uploadData)
{
    m_BufferSize = bufferSize;
    shaderStorageBuffers.resize(m_Config->maxFramesInFlight);
    shaderStorageBuffersMemory.resize(m_Config->maxFramesInFlight);

    // Create a staging buffer used to upload data to the gpu
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_LogicalDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(m_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, uploadData, (size_t)bufferSize);
    vkUnmapMemory(m_LogicalDevice->device, stagingBufferMemory);

    for (size_t i = 0; i < m_Config->maxFramesInFlight; i++) {
        m_LogicalDevice->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            shaderStorageBuffers[i],
            shaderStorageBuffersMemory[i]
        );
        // Copy data from the staging buffer (host) to the shader storage buffer (GPU)
        m_CommandPool->copyBuffer(stagingBuffer, shaderStorageBuffers[i], bufferSize);
    }
    vkDestroyBuffer(m_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice->device, stagingBufferMemory, nullptr);
}

void SSBO::destroy()
{
    vkUnmapMemory(m_LogicalDevice->device, stagingBufferMemory);

    vkDestroyBuffer(m_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice->device, stagingBufferMemory, nullptr);
    vkDestroyBuffer(m_LogicalDevice->device, shaderStorageBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice->device, shaderStorageBufferMemory, nullptr);
}

void SSBO::destroy1()
{
    for (size_t i = 0; i < m_Config->maxFramesInFlight; i++) {
        vkDestroyBuffer(m_LogicalDevice->device, shaderStorageBuffers[i], nullptr);
        vkFreeMemory(m_LogicalDevice->device, shaderStorageBuffersMemory[i], nullptr);
    }
}

void SSBO::update(const void* updateData) {
    // Create a staging buffer used to upload data to the gpu

    //vkMapMemory(m_LogicalDevice->device, stagingBufferMemory, 0, m_BufferSize, 0, &data);
    memcpy(stagingBufferMapped, updateData, (size_t)m_BufferSize);

    // Copy data from the staging buffer (host) to the shader storage buffer (GPU)
    m_CommandPool->copyBuffer(stagingBuffer, shaderStorageBuffer, m_BufferSize);
    
    


    //memcpy(shaderStorageBufferMemoryMapped, updateData, (size_t)m_BufferSize);
}

void SSBO::update1(const void* updateData) {
    // Create a staging buffer used to upload data to the gpu
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_LogicalDevice->createBuffer(
        m_BufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(m_LogicalDevice->device, stagingBufferMemory, 0, m_BufferSize, 0, &data);
    memcpy(data, updateData, (size_t)m_BufferSize);
    vkUnmapMemory(m_LogicalDevice->device, stagingBufferMemory);

    for (size_t i = 0; i < m_Config->maxFramesInFlight; i++) {
        m_LogicalDevice->createBuffer(
            m_BufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            shaderStorageBuffers[i],
            shaderStorageBuffersMemory[i]
        );
        // Copy data from the staging buffer (host) to the shader storage buffer (GPU)
        m_CommandPool->copyBuffer(stagingBuffer, shaderStorageBuffers[i], m_BufferSize);
    }
    vkDestroyBuffer(m_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice->device, stagingBufferMemory, nullptr);
}