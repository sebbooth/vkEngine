#include "CommandBuffers.h"

CommandBuffers::CommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    std::shared_ptr<VkConfig> config
)
{
    m_Device = device;
    m_CommandPool = commandPool;
    m_Config = config;
}

void CommandBuffers::create()
{
    commandBuffers.resize(m_Config->maxFramesInFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(m_Device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}


void CommandBuffers::attachAllResources(
    VkRenderPass renderPass, 
    VkPipeline graphicsPipeline, 
    VkPipelineLayout pipelineLayout, 
    std::vector<VkFramebuffer> frameBuffers, 
    std::vector<VkDescriptorSet> descriptorSets, 
    VkBuffer vertexBuffer, 
    VkBuffer indexBuffer
)
{
    m_RenderPass = renderPass;
    m_GraphicsPipeline = graphicsPipeline;
    m_PipelineLayout = pipelineLayout;
    m_FrameBuffers = frameBuffers;
    m_DescriptorSets = descriptorSets;
    m_VertexBuffer = vertexBuffer;
    m_IndexBuffer = indexBuffer;
}

void CommandBuffers::attachRenderPass(VkRenderPass renderPass)
{
    m_RenderPass = renderPass;
}

void CommandBuffers::attachGraphicsPipeline(VkPipeline graphicsPipeline, VkPipelineLayout pipelineLayout)
{
    m_GraphicsPipeline = graphicsPipeline;
    m_PipelineLayout = pipelineLayout;
}

void CommandBuffers::attachFrameBuffers(std::vector<VkFramebuffer> frameBuffers)
{
    m_FrameBuffers = frameBuffers;
}

void CommandBuffers::attachDescriptorSets(std::vector<VkDescriptorSet> descriptorSets)
{
    m_DescriptorSets = descriptorSets;
}

void CommandBuffers::attachVertexBuffer(VkBuffer vertexBuffer)
{
    m_VertexBuffer = vertexBuffer;
}

void CommandBuffers::attachIndexBuffer(VkBuffer indexBuffer)
{
    m_IndexBuffer = indexBuffer;
}

void CommandBuffers::setExtent(VkExtent2D extent)
{
    m_Extent = extent;
}

void CommandBuffers::attachGui(std::shared_ptr<Gui> p_Gui)
{
    mp_Gui = p_Gui;
}

void CommandBuffers::recordBufferVertices(
    VkCommandBuffer commandBuffer,
    std::vector<VkBuffer> shaderStorageBuffers,
    uint32_t currentFrame,
    uint32_t imageIndex, 
    int vertexCount
)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_FrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_Extent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_Extent.width;
    viewport.height = (float)m_Extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_Extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &shaderStorageBuffers[currentFrame], offsets);

    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandBuffers::recordBufferIndexed(uint32_t currentFrame, uint32_t imageIndex, uint32_t indexCount)
{
    VkCommandBuffer commandBuffer = commandBuffers[currentFrame];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_FrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_Extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_Extent.width;
    viewport.height = (float)m_Extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_Extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { m_VertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        commandBuffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        m_PipelineLayout,
        0, 
        1, 
        &m_DescriptorSets[currentFrame],
        0, 
        nullptr
    );

    vkCmdDrawIndexed(
        commandBuffer, 
        indexCount,
        1, 
        0, 
        0, 
        0
    );

    if (m_Config->guiEnabled) {
        mp_Gui->draw(commandBuffer);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}


void CommandBuffers::recordComputeBuffer(
    VkCommandBuffer commandBuffer,
    std::vector<VkBuffer> shaderStorageBuffers,
    uint32_t currentFrame,
    uint32_t imageIndex,
    int vertexCount
)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_FrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_Extent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_Extent.width;
    viewport.height = (float)m_Extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_Extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &shaderStorageBuffers[currentFrame], offsets);

    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

    
    if (m_Config->guiEnabled) {
        mp_Gui->draw(commandBuffer);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandBuffers::resetBuffer(uint32_t currentFrame)
{
    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
}
