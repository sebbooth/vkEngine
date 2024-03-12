#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "CommandBuffers.h"

class SyncObjects
{
public:
	std::shared_ptr<CommandBuffers> commandBufferObj;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	SyncObjects(std::shared_ptr<CommandBuffers> commandBufferObj);
	void waitForFences(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout);
};

