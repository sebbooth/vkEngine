#pragma once

#ifndef SURFACE_H
#define SURFACE_H

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
	std::shared_ptr<Instance> p_Instance;

	Surface(std::shared_ptr<Instance> p_Instance, GLFWwindow* window);
	
	void destroy();
};

#endif