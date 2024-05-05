#include "SyncObjects.h"

SyncObjects::SyncObjects(VkDevice device, std::shared_ptr<VkConfig> config)
{
    m_Device = device;
    m_Config = config;
}

void SyncObjects::create()
{
    imageAvailableSemaphores.resize(m_Config->maxFramesInFlight);
    renderFinishedSemaphores.resize(m_Config->maxFramesInFlight);
    computeFinishedSemaphores.resize(m_Config->maxFramesInFlight);
    depthComputeFinishedSemaphores.resize(m_Config->maxFramesInFlight);
    inFlightFences.resize(m_Config->maxFramesInFlight);
    computeInFlightFences.resize(m_Config->maxFramesInFlight);
    depthComputeInFlightFences.resize(m_Config->maxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_Config->maxFramesInFlight; i++) {
        if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        }
        if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &depthComputeFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device, &fenceInfo, nullptr, &depthComputeInFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create depth compute synchronization objects for a frame!");
        }
    }
}

void SyncObjects::destroy()
{
    for (size_t i = 0; i < m_Config->maxFramesInFlight; i++) {
        vkDestroySemaphore(m_Device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device, computeFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device, depthComputeFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_Device, inFlightFences[i], nullptr);
        vkDestroyFence(m_Device, computeInFlightFences[i], nullptr);
        vkDestroyFence(m_Device, depthComputeInFlightFences[i], nullptr);
    }
}

void SyncObjects::waitForGraphicsFence(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout)
{
    vkWaitForFences(m_Device, 1, &inFlightFences[currentFrame], waitAll, timeout);
    vkResetFences(m_Device, 1, &inFlightFences[currentFrame]);
}

void SyncObjects::waitForComputeFence(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout)
{
    vkWaitForFences(m_Device, 1, &computeInFlightFences[currentFrame], waitAll, timeout);
    vkResetFences(m_Device, 1, &computeInFlightFences[currentFrame]);
}

void SyncObjects::waitForDepthComputeFence(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout)
{
    vkWaitForFences(m_Device, 1, &depthComputeInFlightFences[currentFrame], waitAll, timeout);
    vkResetFences(m_Device, 1, &depthComputeInFlightFences[currentFrame]);
}

