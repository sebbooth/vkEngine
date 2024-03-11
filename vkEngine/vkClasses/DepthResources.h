#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

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
private:
	std::shared_ptr<RenderPass> renderPassObj;
};

