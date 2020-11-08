#pragma once

#include "../utility/internalptr.hpp"
#include "commandpool.hpp"
#include "device.hpp"
#include "physicaldevice.hpp"
#include "surface.hpp"

#include <vulkan/vulkan.h>

namespace nuvk
{
    class RenderContext
    {
    public:
        static inline constexpr std::string_view Section = "RENDERCONTEXT";

        RenderContext(
            const Device &device,
            const PhysicalDevice &physicalDevice,
            const Surface &surface,
            const CommandPool &commandPool,
            uint32_t width, uint32_t height
        );

        bool beginRender();
        bool endRender();

        VkViewport getViewport() const;
        VkRect2D getScissor() const;
        VkRenderPass getRenderPass() const;
        VkCommandBuffer getActiveCommandBuffer() const;
        VkExtent2D getSwapchainExtent() const;

    private:
        struct Internal;
        InternalPtr<Internal> internal;
    };
}