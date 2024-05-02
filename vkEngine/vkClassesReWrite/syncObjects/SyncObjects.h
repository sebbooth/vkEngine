#pragma once

#ifndef SYNCOBJECTS_H
#define SYNCOBJECTS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <memory>

#include "initialization/VkConfig.h"

class SyncObjects
{
public:
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkSemaphore> computeFinishedSemaphores;
	std::vector<VkSemaphore> depthComputeFinishedSemaphores;

	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> computeInFlightFences;
	std::vector<VkFence> depthComputeInFlightFences;

private:
	VkDevice m_Device;
	std::shared_ptr<VkConfig> m_Config;

public:
	SyncObjects(VkDevice device, std::shared_ptr<VkConfig> config);
	void create();
	void destroy();
	void waitForGraphicsFence(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout);
	void waitForComputeFence(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout);
	void waitForDepthComputeFence(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout);
};

#endif