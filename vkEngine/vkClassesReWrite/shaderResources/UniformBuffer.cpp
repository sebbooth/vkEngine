#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(std::shared_ptr<LogicalDevice> logicalDevice, std::shared_ptr<VkConfig> config)
{
	m_LogicalDevice = logicalDevice;
	m_Config = config;
}

void UniformBuffer::create(VkDeviceSize bufferSize)
{
    uniformBuffers.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    m_BufferSize = bufferSize;

    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        m_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(m_LogicalDevice->device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void UniformBuffer::update(uint32_t currentImage, const void* updateData)
{
    memcpy(uniformBuffersMapped[currentImage], updateData, m_BufferSize);
}

void UniformBuffer::destroy()
{
    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(m_LogicalDevice->device, uniformBuffers[i], nullptr);
        vkFreeMemory(m_LogicalDevice->device, uniformBuffersMemory[i], nullptr);
    }
}
