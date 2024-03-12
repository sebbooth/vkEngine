#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <unordered_map>
#include <tiny_obj_loader.h>

#include "TextureSampler.h"
#include "VkStructs.h"

class Model
{
public:
	std::shared_ptr<TextureSampler> textureSamplerObj;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Model(std::shared_ptr<TextureSampler> textureSamplerObj);
};

