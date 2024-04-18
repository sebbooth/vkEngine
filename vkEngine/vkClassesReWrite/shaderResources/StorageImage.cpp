#include "StorageImage.h"

StorageImage::StorageImage(
    VkDevice device,
    std::shared_ptr<Images> p_Images,
    std::shared_ptr<ImageViews> p_ImageViews, 
    std::shared_ptr<CommandPool> p_CommandPool, 
    std::shared_ptr<VkConfig> config
)
{
    m_Device = device;
    mp_Images = p_Images;
    mp_ImageViews = p_ImageViews;
    mp_CommandPool = p_CommandPool;
    m_Config = config;
}

void StorageImage::create(VkExtent2D extent)
{
    storageImages.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    storageImageMemories.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    storageImageViews.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
    imageSamplers.resize(m_Config->MAX_FRAMES_IN_FLIGHT);

    uint32_t width = extent.width;
    uint32_t height = extent.height;

    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        mp_Images->createImage(
            width,
            height,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            storageImages[i],
            storageImageMemories[i],
            1,
            VK_SAMPLE_COUNT_1_BIT
        );

        transitionImageLayout(
            storageImages[i],
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_GENERAL
        );

        storageImageViews[i] = mp_ImageViews->createImageView(
            storageImages[i],
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1
        );
    }
}

void StorageImage::destroy()
{
    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySampler(m_Device, imageSamplers[i], nullptr);
        vkDestroyImageView(m_Device, storageImageViews[i], nullptr);
        vkDestroyImage(m_Device, storageImages[i], nullptr);
        vkFreeMemory(m_Device, storageImageMemories[i], nullptr);
    }
}

void StorageImage::transitionImageLayout(
    VkImage image, 
    VkFormat format, 
    VkImageLayout oldLayout, 
    VkImageLayout newLayout
)
{
    VkCommandBuffer commandBuffer = mp_CommandPool->beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    mp_CommandPool->endSingleTimeCommands(commandBuffer);
}


