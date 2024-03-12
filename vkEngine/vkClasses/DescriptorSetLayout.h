#pragma once
#include "RenderPass.h"

class DescriptorSetLayout
{
public:
	std::shared_ptr<RenderPass> renderPassObj;

	VkDescriptorSetLayout descriptorSetLayout{};

	DescriptorSetLayout(std::shared_ptr<RenderPass> renderPassObj);
};

