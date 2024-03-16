#include "UniformBuffers.h"

UniformBuffers::UniformBuffers(std::shared_ptr<FrameBuffers> p_FrameBuffers)
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

void UniformBuffers::create()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < p_FrameBuffers->MAX_FRAMES_IN_FLIGHT; i++) {
        p_LogicalDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(p_LogicalDevice->device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void UniformBuffers::update(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), p_SwapChain->swapChainExtent.width / (float)p_SwapChain->swapChainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
