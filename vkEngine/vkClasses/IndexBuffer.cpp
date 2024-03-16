#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::shared_ptr<VertexBuffer> p_VertexBuffer)
{
    this->p_VertexBuffer = p_VertexBuffer;
    this->p_FrameBuffers = p_VertexBuffer->p_FrameBuffers;
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

void IndexBuffer::create(std::vector<uint32_t> indices)
{
    this->indices = indices;

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory;
    p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(p_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(p_LogicalDevice->device, stagingBufferMemory);

    p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
    
    p_CommandPool->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(p_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(p_LogicalDevice->device, stagingBufferMemory, nullptr);
}

void IndexBuffer::destroy()
{
    vkDestroyBuffer(p_LogicalDevice->device, indexBuffer, nullptr);
    vkFreeMemory(p_LogicalDevice->device, indexBufferMemory, nullptr);
}
