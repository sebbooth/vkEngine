#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <memory>

#include "Instance.h"

class Surface
{
public:
	VkSurfaceKHR surface{};
	GLFWwindow* window;
	std::shared_ptr<Instance> instanceObj;

	Surface(std::shared_ptr<Instance> instanceObj, GLFWwindow* window);
};

