#pragma once
#include "UniformBuffers.h"

class DescriptorPool
{
public:
	std::shared_ptr<UniformBuffers> uniformBuffersObj;

	VkDescriptorPool descriptorPool{};

	DescriptorPool(std::shared_ptr<UniformBuffers> uniformBuffersObj);
};

