#include "Surface.h"

Surface::Surface(std::shared_ptr<Instance> p_Instance, GLFWwindow* window)
{
    this->window = window;
    this->p_Instance = p_Instance;

    if (glfwCreateWindowSurface(p_Instance->instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Surface::destroy()
{
    vkDestroySurfaceKHR(p_Instance->instance, surface, nullptr);
}
