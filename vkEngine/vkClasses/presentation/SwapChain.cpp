#include "SwapChain.h"

SwapChain::SwapChain(
    GLFWwindow* window,
    VkSurfaceKHR surface,
    VkDevice device,
    QueueFamilyIndices queueFamilies,
    SwapChainSupportDetails swapChainSupport
)
{
    m_Window = window;
    m_Surface = surface;
    m_Device = device;
    m_QueueFamilies = queueFamilies;
    m_SwapChainSupport = swapChainSupport;
}

void SwapChain::create()
{    
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(m_SwapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(m_SwapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(m_SwapChainSupport.capabilities, m_Window);

    uint32_t imageCount = m_SwapChainSupport.capabilities.minImageCount + 1;

    if (m_SwapChainSupport.capabilities.maxImageCount > 0 && imageCount > m_SwapChainSupport.capabilities.maxImageCount) {
        imageCount = m_SwapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { m_QueueFamilies.graphicsFamily.value(), m_QueueFamilies.presentFamily.value() };

    if (m_QueueFamilies.graphicsFamily != m_QueueFamilies.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = m_SwapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_Device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void SwapChain::destroySwapChain()
{
    vkDestroySwapchainKHR(m_Device, swapChain, nullptr);
}

void SwapChain::updateSwapChainSupport(SwapChainSupportDetails swapChainSupport)
{
    m_SwapChainSupport = swapChainSupport;
}
