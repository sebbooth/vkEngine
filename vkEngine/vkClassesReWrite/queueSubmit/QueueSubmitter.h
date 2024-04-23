#pragma once

#ifndef QUEUESUBMITTER_H
#define QUEUESUBMITTER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

class QueueSubmitter
{
public:
	QueueSubmitter() = default;
	void submit(
		VkQueue queue, 
		VkFence fence, 
		VkCommandBuffer* pCommandBuffers, 
		VkSemaphore* pSignalSemaphores, 
		uint32_t waitsemaphoreCount = 0,
		VkSemaphore* pWaitSemaphores = nullptr,
		VkPipelineStageFlags* pWaitDstStageMask = nullptr
	);
};

#endif // !QUEUESUBMITTER_H