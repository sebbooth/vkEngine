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

void ComputeCommandBuffers::setExtent(unsigned int width, unsigned int height)
{
    groupSizeX = std::ceil(((width / m_Config->downScaleFactor) * (height / m_Config->downScaleFactor)) / 256);
}

void ComputeCommandBuffers::setGroupSize(unsigned int size)
{
    groupSizeX = std::ceil(size / (m_Config->downScaleFactor * m_Config->downScaleFactor) / 256);
}

void ComputeCommandBuffers::recordBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame)
{
    vkResetCommandBuffer(
        commandBuffers[currentFrame],
        /*VkCommandBufferResetFlagBits*/ 0
    );

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
    
    static bool firstRun = true;

    VkMemoryBarrier2 memoryBarrier{};
    if (firstRun) {
        memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE_KHR;
        memoryBarrier.srcAccessMask = VK_ACCESS_2_NONE_KHR;
    }
    else { 
        memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
        memoryBarrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT_KHR | VK_ACCESS_2_SHADER_READ_BIT_KHR;
    }
    memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
    memoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT_KHR | VK_ACCESS_2_SHADER_READ_BIT_KHR;

    VkDependencyInfo dependencyInfo{};
    dependencyInfo.memoryBarrierCount = 1;
    dependencyInfo.pMemoryBarriers = &memoryBarrier;

    //vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

    vkCmdDispatch(commandBuffer, groupSizeX, groupSizeY, groupSizeZ);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
    }
    firstRun = false;
}

