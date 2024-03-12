#pragma once
#include <chrono>
#include "IndexBuffer.h"

class UniformBuffers
{
public:
	std::shared_ptr<IndexBuffer> indexBufferObj;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	UniformBuffers(std::shared_ptr<IndexBuffer> indexBufferObj);
	void update(uint32_t currentImage);
};

