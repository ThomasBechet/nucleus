#pragma once

#include "../utility/glfwinterface.hpp"
#include "../utility/debugutilsmessenger.hpp"
#include "../utility/loggable.hpp"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace nuvk
{
    struct QueueFamilyIndices;

    class Context : public Loggable
    {
    public:
        Context(bool enableValidationLayers = true);
        ~Context();
        
    public:
        const vk::Device &getDevice() const;
        const vk::Queue &getGraphicsQueue() const;
        const vk::Queue &getPresentQueue() const;

    private:
        void createInstance();
        void setupDebugCallback();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();

    public:
        static std::vector<const char*> GetRequiredExtensions(GLFWInterface &glfwInterface, bool enableValidationLayers);
        static std::vector<const char*> GetRequiredValidationLayers();
        static std::vector<const char*> GetRequiredDeviceExtensions();
    private:
        static bool CheckValidationLayerSupport();
        static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
        static bool IsDeviceSuitable(vk::PhysicalDevice device, VkSurfaceKHR surface);
        static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, VkSurfaceKHR surface);

    private:
        std::unique_ptr<GLFWInterface> m_glfwInterface;
        vk::UniqueInstance m_instance;
        std::unique_ptr<DebugUtilsMessenger> m_debugUtilsMessenger;
        VkSurfaceKHR m_surface;

        vk::PhysicalDevice m_physicalDevice;
        vk::UniqueDevice m_device;

        vk::Queue m_graphicsQueue;
        vk::Queue m_presentQueue;

        bool m_useValidationLayers;
    };
}