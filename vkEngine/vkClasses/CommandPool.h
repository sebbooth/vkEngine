#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "GraphicsPipeline.h"

class CommandPool
{
public:
	std::shared_ptr<GraphicsPipeline> graphicsPipelineObj;

	VkCommandPool commandPool{};

	CommandPool(std::shared_ptr<GraphicsPipeline> graphicsPipelineObj);
};

