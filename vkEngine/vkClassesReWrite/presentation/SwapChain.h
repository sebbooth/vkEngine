#pragma once

#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

#include "VkStructs.h"
#include "SwapChainUtils.h"

class SwapChain
{
	public:
		VkSwapchainKHR swapChain{};
		VkFormat swapChainImageFormat{};
		VkExtent2D swapChainExtent{};
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

	private:
		GLFWwindow* m_Window;
		VkSurfaceKHR m_Surface{};
		VkDevice m_Device{};
		QueueFamilyIndices m_QueueFamilies{};
		SwapChainSupportDetails m_SwapChainSupport{};

	public:
		SwapChain(
			GLFWwindow* window,
			VkSurfaceKHR surface,
			VkDevice device,
			QueueFamilyIndices queueFamilies,
			SwapChainSupportDetails swapChainSupport
		);
		void create();
		void destroySwapChain();
};

#endif