#pragma once

#include "vulkan.hpp"

#include <vk_mem_alloc.h>

namespace vkx
{
    struct Allocation
    {
        VmaAllocation allocation;
    };

    struct AllocatedBuffer : public Allocation
    {
        vk::Buffer buffer;
    };

    struct AllocatedImage : public Allocation
    {
        vk::Format format;
        vk::Image image;
    };
} // namespace vkx
