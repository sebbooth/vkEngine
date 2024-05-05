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
#include "initialization/VkConfig.h"

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
    std::shared_ptr<VkConfig> m_Config;

public:
    LogicalDevice(
        std::shared_ptr<PhysicalDevice> p_PhysicalDevice,
        std::shared_ptr<VkConfig> config
    );

    void create();
    void destroy();

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