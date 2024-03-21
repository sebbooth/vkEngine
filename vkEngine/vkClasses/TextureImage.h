#pragma once

#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include <stb_image.h>
#include "FrameBuffers.h"

class TextureImage
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;
	std::shared_ptr<ImageViews> p_ImageViews;
	std::shared_ptr<RenderPass> p_RenderPass;
	std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout;
	std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
	std::shared_ptr<CommandPool> p_CommandPool;
	std::shared_ptr<FrameBuffers> p_FrameBuffers;

	uint32_t mipLevels = 1;
	VkImage textureImage{};
	VkDeviceMemory textureImageMemory{};
	VkImageView textureImageView{};

	bool mipmapsEnabled = true;

	TextureImage(std::shared_ptr<FrameBuffers> p_FrameBuffers);
	void create();

	void createTextureImageView();

	const std::string TEXTURE_PATH = "assets/models/viking_room/viking_room.png";

private:
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
};

#endif