#pragma once
#include "TextureImage.h"

class TextureSampler
{
public:
	std::shared_ptr<TextureImage> textureImageObj;

	VkSampler textureSampler{};

	TextureSampler(std::shared_ptr<TextureImage> textureImageObj);
};

