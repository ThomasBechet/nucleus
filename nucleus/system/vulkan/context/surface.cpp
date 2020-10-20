#include "surface.hpp"

#include "../utility/glfwinterface.hpp"
#include "engine.hpp"
#include "surface.hpp"

using namespace nuvk;

namespace
{
    static VkSurfaceKHR CreateSurface(
        vk::UniqueInstance &instance,
        GLFWInterface &glfwInterface
    )
    {
        VkSurfaceKHR rawSurface;
        VkInstance vkinstance = *instance;
        if (glfwInterface->create_window_surface(&vkinstance, &rawSurface) != NU_SUCCESS) {
            Engine::Interrupt("Failed to create surface from GLFW.");
        }
        return rawSurface;
    }
}

struct Surface::Internal
{
    vk::SurfaceKHR surface;
    vk::UniqueInstance &instance;

    Internal(
        vk::UniqueInstance &instance,
        GLFWInterface &glfwInterface
    ) : instance(instance)
    {
        surface = ::CreateSurface(instance, glfwInterface);
    }
    ~Internal()
    {
        instance->destroySurfaceKHR(surface);
    }
};

Surface::Surface(
    vk::UniqueInstance &instance,
    GLFWInterface &glfwInterface
) : internal(MakeInternalPtr<Internal>(instance, glfwInterface)) {}

VkSurfaceKHR Surface::getSurface()
{
    return internal->surface;
}