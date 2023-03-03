#pragma once

#include "allocations.hpp"

#include <deque>
#include <functional>

namespace vkx
{
    class Allocator
    {
    public:
        Allocator(VmaAllocatorCreateInfo alloc_info);
        ~Allocator();

        VmaAllocator const& get() const;

        void free();

        AllocatedBuffer create_buffer(vk::BufferCreateInfo buffer_info,
                                      VmaAllocationCreateInfo alloc_info);
        AllocatedImage create_image(vk::ImageCreateInfo img_info,
                                    VmaAllocationCreateInfo alloc_info,
                                    vk::Format format);

    private:
        VmaAllocator m_allocator;
        std::deque<std::function<void()>> m_deleters;
    };

} // namespace vkx
