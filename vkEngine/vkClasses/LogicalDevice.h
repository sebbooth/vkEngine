#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "PhysicalDevice.h"

class LogicalDevice
{
public:
	VkDevice device{};
	VkQueue graphicsQueue{};
	VkQueue presentQueue{};
	std::shared_ptr<PhysicalDevice> physicalDeviceObj;

	LogicalDevice(std::shared_ptr<PhysicalDevice> physicalDeviceObj);

    void createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory
        );

    void createBuffer(
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties, 
        VkBuffer& buffer, 
        VkDeviceMemory& bufferMemory
    );

    void waitIdle();
};

