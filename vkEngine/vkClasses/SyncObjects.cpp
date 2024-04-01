#include "SyncObjects.h"

SyncObjects::SyncObjects(std::shared_ptr<CommandBuffers> p_CommandBuffer)
{
    this->p_CommandBuffer = p_CommandBuffer;
    this->p_DescriptorSets = p_CommandBuffer->p_DescriptorSets;
    this->p_DescriptorPool = p_DescriptorSets->p_DescriptorPool;
    this->p_FrameBuffers = p_DescriptorPool->p_FrameBuffers;
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

void SyncObjects::create()
{
    imageAvailableSemaphores.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    computeFinishedSemaphores.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);
    computeInFlightFences.resize(p_FrameBuffers->MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < p_FrameBuffers->MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(p_LogicalDevice->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(p_LogicalDevice->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(p_LogicalDevice->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        if (vkCreateSemaphore(p_LogicalDevice->device, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(p_LogicalDevice->device, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        }
    }
}

void SyncObjects::destroy()
{
    for (size_t i = 0; i < p_FrameBuffers->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(p_LogicalDevice->device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(p_LogicalDevice->device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(p_LogicalDevice->device, computeFinishedSemaphores[i], nullptr);
        vkDestroyFence(p_LogicalDevice->device, inFlightFences[i], nullptr);
        vkDestroyFence(p_LogicalDevice->device, computeInFlightFences[i], nullptr);
    }
}

void SyncObjects::waitForFences(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout)
{
    vkWaitForFences(p_LogicalDevice->device, 1, &inFlightFences[currentFrame], waitAll, timeout);
}
