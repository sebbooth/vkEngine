#pragma once
#ifndef SSBO_H
#define SSBO_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "initialization/VkConfig.h"
#include "devices/LogicalDevice.h"
#include "commands/CommandPool.h"

class SSBO
{
	public:
		std::vector<VkBuffer> shaderStorageBuffers;
		std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

		VkBuffer shaderStorageBuffer;
		VkDeviceMemory shaderStorageBufferMemory;
		void* shaderStorageBufferMemoryMapped;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		void* stagingBufferMapped;

	private:
		std::shared_ptr<LogicalDevice> m_LogicalDevice;
		std::shared_ptr<CommandPool> m_CommandPool;
		std::shared_ptr<VkConfig> m_Config;
		VkDeviceSize m_BufferSize;

	public:
		SSBO(
			std::shared_ptr<LogicalDevice> logicalDevice,
			std::shared_ptr<CommandPool> commandPool,
			std::shared_ptr<VkConfig> config
		);

		void uploadData(
			VkDeviceSize bufferSize,
			const void* uploadData
		);

		void uploadData1(
			VkDeviceSize bufferSize,
			const void* uploadData
		);
		void update1(const void* updateData);

		void destroy();
		void destroy1();
		void update(const void* updateData);
};

#endif // !SSBO_H
