#pragma once
#include "LogicalDevice.h"
#include "SwapChainUtils.h"

class SwapChain
{
public:
	VkSwapchainKHR swapChain{};
	VkFormat swapChainImageFormat{};
	VkExtent2D swapChainExtent{};
	std::vector<VkImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	std::shared_ptr<LogicalDevice> logicalDeviceObj;

	SwapChain(std::shared_ptr<LogicalDevice> logicalDeviceObj);
	void create();
	void destroyFramebuffers();
	void destroySwapChain();
};

