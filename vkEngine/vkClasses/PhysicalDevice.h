#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
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
	VkPhysicalDevice physicalDevice{};
	std::shared_ptr<Surface> surfaceObj;
	SwapChainSupportDetails swapChainSupportDetails;
	QueueFamilyIndices queueFamilies;

	PhysicalDevice(std::shared_ptr<Surface> surfaceObj);

	SwapChainSupportDetails getSwapChainSupportDetails();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);

};
