#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "RenderPass.h"

class DescriptorSetLayout
{
public:
	std::shared_ptr<RenderPass> renderPassObj;

	VkDescriptorSetLayout descriptorSetLayout{};

	DescriptorSetLayout(std::shared_ptr<RenderPass> renderPassObj);
};

