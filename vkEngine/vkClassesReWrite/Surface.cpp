#include "Surface.h"

Surface::Surface(VkInstance instance, GLFWwindow* window)
{
    this->window = window;
    this->instance = instance;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Surface::destroy()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
