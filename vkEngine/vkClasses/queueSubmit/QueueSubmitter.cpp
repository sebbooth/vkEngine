#include "QueueSubmitter.h"


void QueueSubmitter::submit(
	VkQueue queue,
	VkFence fence,
	VkCommandBuffer* pCommandBuffers, 
	VkSemaphore* pSignalSemaphores, 
	uint32_t waitsemaphoreCount, 
	VkSemaphore* pWaitSemaphores, 
	VkPipelineStageFlags* pWaitDstStageMask
)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = pCommandBuffers;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = pSignalSemaphores;
	if (waitsemaphoreCount > 0) {
		submitInfo.waitSemaphoreCount = waitsemaphoreCount;
		submitInfo.pWaitSemaphores = pWaitSemaphores;
		submitInfo.pWaitDstStageMask = pWaitDstStageMask;
	}

	if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit compute command buffer!");
	};
}
