#pragma once
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"

class CommandPool
{
public:
	std::shared_ptr<GraphicsPipeline> graphicsPipelineObj;

	VkCommandPool commandPool{};

	CommandPool(std::shared_ptr<GraphicsPipeline> graphicsPipelineObj);

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


private:
	std::shared_ptr<LogicalDevice> logicalDeviceObj;
};

