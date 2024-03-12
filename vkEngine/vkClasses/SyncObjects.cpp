#include "SyncObjects.h"

SyncObjects::SyncObjects(std::shared_ptr<CommandBuffers> commandBufferObj)
{
    this->commandBufferObj = commandBufferObj;

    std::shared_ptr<UniformBuffers> uniformBuffersObj = commandBufferObj->
        descriptorSetsObj->
        descriptorPoolObj->
        uniformBuffersObj;

    std::shared_ptr<LogicalDevice> logicalDeviceObj = uniformBuffersObj->
        indexBufferObj->
        vertexBufferObj->
        modelObj->
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

    imageAvailableSemaphores.resize(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < uniformBuffersObj->MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicalDeviceObj->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDeviceObj->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDeviceObj->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void SyncObjects::waitForFences(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout)
{
    std::shared_ptr<LogicalDevice> logicalDeviceObj = commandBufferObj->
        descriptorSetsObj->
        descriptorPoolObj->
        uniformBuffersObj->
        indexBufferObj->
        vertexBufferObj->
        modelObj->
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

    vkWaitForFences(logicalDeviceObj->device, 1, &inFlightFences[currentFrame], waitAll, timeout);
}
