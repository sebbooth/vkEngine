#pragma once

#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <set>
#include <stdexcept>

#include "PhysicalDevice.h"
#include "VkStructs.h"
#include "RenderingSettings.h"

class LogicalDevice
{
public:
	VkDevice device{};
	VkQueue graphicsQueue{};
    VkQueue presentQueue{};
    VkQueue computeQueue{};


    bool wireFrameEnabled = false;


private:
    std::shared_ptr<PhysicalDevice> mp_PhysicalDevice;
    std::shared_ptr<RenderingSettings> m_RS;

public:
	LogicalDevice(
        std::shared_ptr<PhysicalDevice> p_PhysicalDevice,
        std::shared_ptr<RenderingSettings> rs
    );

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