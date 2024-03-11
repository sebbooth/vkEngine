#include "CommandPool.h"

CommandPool::CommandPool(std::shared_ptr<GraphicsPipeline> graphicsPipelineObj)
{
    this->graphicsPipelineObj = graphicsPipelineObj;

    QueueFamilyIndices queueFamilyIndices = graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->imageViewsObj->swapChainObj->logicalDeviceObj->physicalDeviceObj->queueFamilies;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->imageViewsObj->swapChainObj->logicalDeviceObj->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}
