#pragma once
#include <stdexcept>
#include <vector>
#include <set>
#include <string>
#include <memory>

#include "VkStructs.h"
#include "Surface.h"

class PhysicalDevice
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;

	VkPhysicalDevice physicalDevice{};

	SwapChainSupportDetails swapChainSupportDetails;
	QueueFamilyIndices queueFamilies;
		
	bool msaaEnabled = false;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	PhysicalDevice(std::shared_ptr<Surface> p_Surface);

	SwapChainSupportDetails getSwapChainSupportDetails();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void create();

private:
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	VkSampleCountFlagBits getMaxUsableSampleCount();

};
