#include "CommandBuffers.h"

CommandBuffers::CommandBuffers(std::shared_ptr<DescriptorSets> descriptorSetsObj)
{
	this->descriptorSetsObj = descriptorSetsObj;

    std::shared_ptr<UniformBuffers> uniformBuffersObj = descriptorSetsObj->
        descriptorPoolObj->
        uniformBuffersObj;

    std::shared_ptr<CommandPool> commandPoolObj = uniformBuffersObj->
        indexBufferObj->
        vertexBufferObj->
        modelObj->textureSamplerObj->
        textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        colorResourcesObj->
        commandPoolObj;

    std::shared_ptr<LogicalDevice> logicalDeviceObj = commandPoolObj->
        graphicsPipelineObj->
        descriptorSetLayoutObj->
        renderPassObj->
        imageViewsObj->
        swapChainObj->
        logicalDeviceObj;

    commandBuffers.resize(uniformBuffersObj->MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPoolObj->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(logicalDeviceObj->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandBuffers::recordBuffer(uint32_t currentFrame, uint32_t imageIndex)
{
    std::shared_ptr<IndexBuffer> indexBufferObj = descriptorSetsObj->
        descriptorPoolObj->
        uniformBuffersObj->
        indexBufferObj;
    
    std::shared_ptr<GraphicsPipeline> graphicsPipelineObj = indexBufferObj->
        vertexBufferObj->
        modelObj->
        textureSamplerObj->
        textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        colorResourcesObj->
        commandPoolObj->
        graphicsPipelineObj;

    std::shared_ptr<SwapChain> swapChainObj = graphicsPipelineObj->
        descriptorSetLayoutObj->
        renderPassObj->
        imageViewsObj->
        swapChainObj;

    VkCommandBuffer commandBuffer = commandBuffers[currentFrame];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->renderPass;
    renderPassInfo.framebuffer = swapChainObj->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainObj->swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineObj->graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainObj->swapChainExtent.width;
    viewport.height = (float)swapChainObj->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainObj->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { indexBufferObj->vertexBufferObj->vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, indexBufferObj->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineObj->pipelineLayout, 0, 1, &descriptorSetsObj->descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexBufferObj->vertexBufferObj->modelObj->indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandBuffers::resetBuffer(uint32_t currentFrame)
{
    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
}
