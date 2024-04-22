#include "ComputeCommandBuffers.h"

ComputeCommandBuffers::ComputeCommandBuffers(
	VkDevice device, 
	VkCommandPool commandPool, 
	std::shared_ptr<VkConfig> config
)
{
	m_Device = device;
	m_CommandPool = commandPool;
	m_Config = config;
}

void ComputeCommandBuffers::create()
{
    commandBuffers.resize(m_Config->maxFramesInFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(m_Device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute command buffers!");
    }
}

void ComputeCommandBuffers::attachComputePipeline(VkPipeline computePipeline, VkPipelineLayout pipelineLayout)
{
    m_ComputePipeline = computePipeline;
    m_PipelineLayout = pipelineLayout;
}

void ComputeCommandBuffers::attachDescriptorSets(std::vector<VkDescriptorSet> descriptorSets)
{
    m_DescriptorSets = descriptorSets;
}

void ComputeCommandBuffers::setExtent(VkExtent2D extent)
{
    groupSizeX = std::ceil(((extent.width / m_Config->downScaleFactor) * (extent.height / m_Config->downScaleFactor)) / 256 + 1);
}

void ComputeCommandBuffers::recordBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording compute command buffer!");
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline);

    vkCmdBindDescriptorSets(
        commandBuffer, 
        VK_PIPELINE_BIND_POINT_COMPUTE, 
        m_PipelineLayout,
        0, 
        1, 
        &m_DescriptorSets[currentFrame],
        0, 
        nullptr
    );

    vkCmdDispatch(commandBuffer, groupSizeX, groupSizeY, groupSizeZ);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
    }
}
