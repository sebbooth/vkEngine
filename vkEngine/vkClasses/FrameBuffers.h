#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

#include "DepthResources.h"

class FrameBuffers
{
public:
	std::shared_ptr<DepthResources> depthResourcesObj;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	FrameBuffers(std::shared_ptr<DepthResources> depthResourcesObj);
	void create();
};

