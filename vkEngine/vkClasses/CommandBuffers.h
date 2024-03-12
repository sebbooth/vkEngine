#pragma once
#include "DescriptorSets.h"

class CommandBuffers
{
public:
	std::shared_ptr<DescriptorSets> descriptorSetsObj;

	std::vector<VkCommandBuffer> commandBuffers;

	CommandBuffers(std::shared_ptr<DescriptorSets> descriptorSetsObj);

	void recordBuffer(uint32_t currentFrame, uint32_t imageIndex);
	void resetBuffer(uint32_t currentFrame);
};

