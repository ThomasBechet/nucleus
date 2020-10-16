#pragma once

#include "../utility/internalptr.hpp"

#include <vulkan/vulkan.hpp>

namespace nuvk
{
    class Surface
    {
    public:
        inline constexpr auto Section = "SURFACE";

        vk::SurfaceKHR &getSurface();

    private:
        struct Internal;
        InternalPtr<Internal> internal;
    };
}