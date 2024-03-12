#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "VertexBuffer.h"

class IndexBuffer
{
public:
	std::shared_ptr<VertexBuffer> vertexBufferObj;

	VkBuffer indexBuffer{};
	VkDeviceMemory indexBufferMemory{};

	IndexBuffer(std::shared_ptr<VertexBuffer> vertexBufferObj);
};

