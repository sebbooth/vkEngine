#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <stb_image.h>

#include "FrameBuffers.h"

class TextureImage
{
public:
	std::shared_ptr<FrameBuffers> frameBuffersObj;

	VkImage textureImage{};
	VkDeviceMemory textureImageMemory{};
	VkImageView textureImageView{};

	TextureImage(std::shared_ptr<FrameBuffers> frameBuffersObj);
	void createTextureImageView();

	const std::string MODEL_PATH = "assets/models/viking_room/viking_room.obj";
	const std::string TEXTURE_PATH = "assets/models/viking_room/viking_room.png";

private:
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};

