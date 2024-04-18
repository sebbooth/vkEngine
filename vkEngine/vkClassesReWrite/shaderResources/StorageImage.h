#pragma once
#ifndef STORAGEIMAGE_H
#define STORAGEIMAGE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "initialization/VkConfig.h"
#include "images/Images.h"
#include "images/ImageViews.h"
#include "commands/CommandPool.h"

class StorageImage
{
    public:
        std::vector<VkImage> storageImages;
        std::vector<VkDeviceMemory> storageImageMemories;
        std::vector<VkImageView> storageImageViews;
        std::vector<VkSampler> imageSamplers;

    private:
        VkDevice m_Device;
        std::shared_ptr<Images> mp_Images;
        std::shared_ptr<ImageViews> mp_ImageViews;
        std::shared_ptr<CommandPool> mp_CommandPool;
        std::shared_ptr<VkConfig> m_Config;

    public:
        StorageImage(
            VkDevice device,
            std::shared_ptr<Images> p_Images,
            std::shared_ptr<ImageViews> p_ImageViews,
            std::shared_ptr<CommandPool> p_CommandPool,
            std::shared_ptr<VkConfig> config
        );

        void create(VkExtent2D extent);
        void destroy();

        void transitionImageLayout(
            VkImage image, 
            VkFormat format, 
            VkImageLayout oldLayout, 
            VkImageLayout newLayout
        );

};

#endif // !STORAGEIMAGE_H
