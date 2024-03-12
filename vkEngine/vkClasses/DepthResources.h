#pragma once
#include "ColorResources.h"

class DepthResources
{
public:
	std::shared_ptr<ColorResources> colorResourcesObj;

	VkImage depthImage{};
	VkDeviceMemory depthImageMemory{};
	VkImageView depthImageView{};

	DepthResources(std::shared_ptr<ColorResources> colorResourcesObj);
	void create();
};

