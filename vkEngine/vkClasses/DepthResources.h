#pragma once
#include "CommandPool.h"

class DepthResources
{
public:
	std::shared_ptr<CommandPool> commandPoolObj;

	VkImage depthImage{};
	VkDeviceMemory depthImageMemory{};
	VkImageView depthImageView{};

	DepthResources(std::shared_ptr<CommandPool> commandPoolObj);
	void create();
};

