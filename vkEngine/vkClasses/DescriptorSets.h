#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "DescriptorPool.h"

class DescriptorSets
{
public:
	std::shared_ptr<DescriptorPool> descriptorPoolObj;

	std::vector<VkDescriptorSet> descriptorSets;

	DescriptorSets(std::shared_ptr<DescriptorPool> descriptorPoolObj);
};

