#pragma once
#include "Model.h"

class VertexBuffer
{
public:
	std::shared_ptr<Model> modelObj;

	VkBuffer vertexBuffer{};
	VkDeviceMemory vertexBufferMemory{};

	VertexBuffer(std::shared_ptr<Model> modelObj);
};

