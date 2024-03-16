#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(std::shared_ptr<FrameBuffers> p_FrameBuffers)
{
    this->p_FrameBuffers = p_FrameBuffers;
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

void VertexBuffer::create(std::vector<Vertex> vertices)
{
    this->vertices = vertices;

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(p_LogicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(p_LogicalDevice->device, stagingBufferMemory);

    p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    p_CommandPool->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(p_LogicalDevice->device, stagingBuffer, nullptr);
    vkFreeMemory(p_LogicalDevice->device, stagingBufferMemory, nullptr);
}

void VertexBuffer::destroy()
{
    vkDestroyBuffer(p_LogicalDevice->device, vertexBuffer, nullptr);
    vkFreeMemory(p_LogicalDevice->device, vertexBufferMemory, nullptr);
}
