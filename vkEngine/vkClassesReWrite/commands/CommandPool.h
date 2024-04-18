#pragma once

#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

#include "VkStructs.h"

class CommandPool
{
	public:
		VkCommandPool commandPool{};
	
	private:
		QueueFamilyIndices m_QueueFamilies;
		VkDevice m_Device;
		VkQueue m_GraphicsQueue;

	public:
		CommandPool(
			QueueFamilyIndices queueFamilies,
			VkDevice device,
			VkQueue graphicsQueue
		);

		void create();
		void destroy();

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

#endif