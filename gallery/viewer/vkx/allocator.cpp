#include "allocator.hpp"

#include "type_cast.hpp"

#include <zeus/assert.hpp>

namespace vkx
{
    Allocator::Allocator(VmaAllocatorCreateInfo alloc_info)
    {
        if (vmaCreateAllocator(&alloc_info, &m_allocator) != VK_SUCCESS)
        {
            throw std::runtime_error{"error: unable to initialise VMA"};
        }
    }

    Allocator::~Allocator()
    {
        vmaDestroyAllocator(m_allocator);
    }

    VmaAllocator const& Allocator::get() const
    {
        return m_allocator;
    }

    void Allocator::free()
    {
        while (!m_deleters.empty())
        {
            m_deleters.back()();
            m_deleters.pop_back();
        }
    }

    AllocatedBuffer Allocator::create_buffer(vk::BufferCreateInfo buffer_info,
                                             VmaAllocationCreateInfo alloc_info)
    {
        AllocatedBuffer buffer;
        if (vmaCreateBuffer(m_allocator,
                            to_vkc_ptr(&buffer_info),
                            &alloc_info,
                            to_vkc_ptr(&buffer.buffer),
                            &buffer.allocation,
                            nullptr)
            != VK_SUCCESS)
        {
            ASSERT(0);
            throw std::runtime_error{"error: buffer creation failed"};
        }

        m_deleters.emplace_back([this, buffer]() {
            vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
        });

        return buffer;
    }

    AllocatedImage Allocator::create_image(vk::ImageCreateInfo img_info,
                                           VmaAllocationCreateInfo alloc_info,
                                           vk::Format format)
    {
        AllocatedImage image;
        if (vmaCreateImage(m_allocator,
                           to_vkc_ptr(&img_info),
                           &alloc_info,
                           to_vkc_ptr(&image.image),
                           &image.allocation,
                           nullptr)
            != VK_SUCCESS)
        {
            ASSERT(0);
            throw std::runtime_error{"error: image creation failed"};
        }

        image.format = format;

        m_deleters.emplace_back([this, image]() {
            vmaDestroyImage(m_allocator, image.image, image.allocation);
        });

        return image;
    }

} // namespace vkx
