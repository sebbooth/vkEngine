#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <iostream>

class Instance
{
public:
	VkInstance instance{};
	VkDebugUtilsMessengerEXT debugMessenger{};

	bool enableValidationLayers;
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;
	
	Instance(bool enableValidationLayers, std::vector<const char*> validationLayers, std::vector<const char*> deviceExtensions);
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