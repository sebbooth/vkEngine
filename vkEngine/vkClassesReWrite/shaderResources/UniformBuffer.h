#pragma once
#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "initialization/VkConfig.h"
#include "devices/LogicalDevice.h"
#include "commands/CommandPool.h"

class UniformBuffer
{
public:
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

private:
    std::shared_ptr<LogicalDevice> m_LogicalDevice;
    std::shared_ptr<VkConfig> m_Config;

    VkDeviceSize m_BufferSize = 0;

public:
    UniformBuffer(
        std::shared_ptr<LogicalDevice> logicalDevice,
        std::shared_ptr<VkConfig> config
    );

    void create(VkDeviceSize bufferSize);
    void update(uint32_t currentImage, const void* updateData);
    void destroy();
};

#endif // !UNIFORMBUFFER_H
