#pragma once

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "devices/LogicalDevice.h"
#include "commands/CommandPool.h"

#include "VkStructs.h"

class VertexBuffer
{
public:
	VkBuffer vertexBuffer{};
	VkDeviceMemory vertexBufferMemory{};

	std::vector<Vertex> vertices;

private:
	std::shared_ptr<LogicalDevice> mp_LogicalDevice;
	std::shared_ptr<CommandPool> mp_CommandPool;

public:
	VertexBuffer(
		std::shared_ptr<LogicalDevice> p_LogicalDevice,
		std::shared_ptr<CommandPool> p_CommandPool
	);

	void create(std::vector<Vertex> vertices);
	void destroy();
};

#endif