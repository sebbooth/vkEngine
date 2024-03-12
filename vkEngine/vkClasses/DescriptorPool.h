#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "UniformBuffers.h"

class DescriptorPool
{
public:
	std::shared_ptr<UniformBuffers> uniformBuffersObj;

	VkDescriptorPool descriptorPool{};

	DescriptorPool(std::shared_ptr<UniformBuffers> uniformBuffersObj);
};

