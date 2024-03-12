#pragma once
#include "DescriptorPool.h"

class DescriptorSets
{
public:
	std::shared_ptr<DescriptorPool> descriptorPoolObj;

	std::vector<VkDescriptorSet> descriptorSets;

	DescriptorSets(std::shared_ptr<DescriptorPool> descriptorPoolObj);
};

