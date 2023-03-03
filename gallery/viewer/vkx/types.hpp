#pragma once

#include "vulkan.hpp"

namespace vkx
{
    struct Queue
    {
        vk::Queue queue;
        std::uint32_t family_index{0};
    };
} // namespace vkx
