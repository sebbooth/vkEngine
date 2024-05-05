#pragma once

#ifndef IMAGES_H
#define IMAGES_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "devices/PhysicalDevice.h"

class Images
{
public:

private:
    std::shared_ptr<PhysicalDevice> mp_PhysicalDevice;
    VkDevice m_Device;

public:
    Images(std::shared_ptr<PhysicalDevice> p_PhysicalDevice, VkDevice device);
    
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
};

#endif // !IMAGES_H
