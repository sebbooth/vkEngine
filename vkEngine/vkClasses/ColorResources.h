#pragma once
#include "CommandPool.h"

class ColorResources
{
public:
	std::shared_ptr<CommandPool> commandPoolObj;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	ColorResources(std::shared_ptr<CommandPool> commandPoolObj);

	void create();
};

