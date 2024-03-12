#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "TextureImage.h"

class TextureSampler
{
public:
	std::shared_ptr<TextureImage> textureImageObj;

	VkSampler textureSampler{};

	TextureSampler(std::shared_ptr<TextureImage> textureImageObj);
};

