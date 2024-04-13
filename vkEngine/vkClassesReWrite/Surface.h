#pragma once

#ifndef SURFACE_H
#define SURFACE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

class Surface
{
public:
	VkSurfaceKHR surface{};
	GLFWwindow* window;
	VkInstance instance;

public:
	Surface(VkInstance instance, GLFWwindow* window);
	void destroy();
};

#endif