#pragma once
#include "VertexBuffer.h"

class IndexBuffer
{
public:
	std::shared_ptr<VertexBuffer> vertexBufferObj;

	VkBuffer indexBuffer{};
	VkDeviceMemory indexBufferMemory{};

	IndexBuffer(std::shared_ptr<VertexBuffer> vertexBufferObj);
};

