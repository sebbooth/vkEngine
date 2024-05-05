#pragma once

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "devices/LogicalDevice.h"
#include "commands/CommandPool.h"

class IndexBuffer
{
public:
	VkBuffer indexBuffer{};
	VkDeviceMemory indexBufferMemory{};

	std::vector<uint32_t> indices;

private:
	std::shared_ptr<LogicalDevice> mp_LogicalDevice;
	std::shared_ptr<CommandPool> mp_CommandPool;

public:
	IndexBuffer(
		std::shared_ptr<LogicalDevice> p_LogicalDevice,
		std::shared_ptr<CommandPool> p_CommandPool
	);
	void create(std::vector<uint32_t> indices);
	void destroy();
};

#endif