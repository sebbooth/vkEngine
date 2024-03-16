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

	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;

	SwapChain(std::shared_ptr<LogicalDevice> p_LogicalDevice);
	void create();
	void destroyFramebuffers();
	void destroySwapChain();
};

