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
    shaderStorageBuffers.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    shaderStorageBuffersMemory.resize(m_Config->MAX_FRAMES_IN_FLIGHT);

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

    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
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
    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(m_LogicalDevice->device, shaderStorageBuffers[i], nullptr);
        vkFreeMemory(m_LogicalDevice->device, shaderStorageBuffersMemory[i], nullptr);
    }
}
