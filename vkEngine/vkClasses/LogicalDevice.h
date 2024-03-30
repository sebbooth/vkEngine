#pragma once

#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#include "PhysicalDevice.h"

class LogicalDevice
{
public:
	VkDevice device{};
	VkQueue graphicsQueue{};
    VkQueue presentQueue{};
    VkQueue computeQueue{};

    std::shared_ptr<Instance> p_Instance;
    std::shared_ptr<Surface> p_Surface;
    std::shared_ptr<PhysicalDevice> p_PhysicalDevice;

    bool wireFrameEnabled = false;

	LogicalDevice(std::shared_ptr<PhysicalDevice> p_PhysicalDevice);

    void create();
    void destroy();

    void createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory,
        uint32_t mipLevels,
        VkSampleCountFlagBits numSamples
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

#endif