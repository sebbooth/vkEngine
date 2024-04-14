#pragma once

#ifndef INSTANCE_H
#define INSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "RenderingSettings.h"

class Instance
{
	public:
		VkInstance instance{};
		VkDebugUtilsMessengerEXT debugMessenger{};
		VkAllocationCallbacks* allocator = nullptr;

	private:
		std::shared_ptr<RenderingSettings> m_RS;
	



	public:
		Instance(std::shared_ptr<RenderingSettings> rs);
		void create();
		void destroy();

		void createDebugMessenger();

		void populateDebugMessengerCreateInfo(
			VkDebugUtilsMessengerCreateInfoEXT& createInfo
		);

		VkResult CreateDebugUtilsMessengerEXT(
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		);

		void DestroyDebugUtilsMessengerEXT(
			const VkAllocationCallbacks* pAllocator
		);

	private:
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();
};




static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << std::endl << "\033[1;31m[VALIDATION LAYER]: \033[0m" << pCallbackData->pMessage;
	return VK_FALSE;
}

#endif