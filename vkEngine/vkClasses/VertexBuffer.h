#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "Model.h"

class VertexBuffer
{
public:
	std::shared_ptr<Model> modelObj;

	VkBuffer vertexBuffer{};
	VkDeviceMemory vertexBufferMemory{};

	VertexBuffer(std::shared_ptr<Model> modelObj);
};

