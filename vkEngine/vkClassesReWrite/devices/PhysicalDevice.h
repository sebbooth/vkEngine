#pragma once

#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <stdexcept>
#include <vector>
#include <set>
#include <string>
#include <memory>

#include "VkStructs.h"
#include "initialization/VkConfig.h"

class PhysicalDevice
{
	public:
		VkPhysicalDevice physicalDevice{};

		SwapChainSupportDetails swapChainSupportDetails;
		QueueFamilyIndices queueFamilies;
		
		uint32_t presentFamily;
		uint32_t graphicsFamily;
		uint32_t graphicsAndComputeFamily;

		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	private:
		VkInstance m_Instance{};
		VkSurfaceKHR m_Surface{};
		std::shared_ptr<VkConfig> m_Config;

	public:
		PhysicalDevice(
			VkInstance instance, 
			VkSurfaceKHR surface, 
			std::shared_ptr<VkConfig> config
		);

		SwapChainSupportDetails getSwapChainSupportDetails();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void create();

	private:
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		VkSampleCountFlagBits getMaxUsableSampleCount();

};

#endif