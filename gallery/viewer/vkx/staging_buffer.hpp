#pragma once

#include "allocations.hpp"

namespace vkx
{
    class StagingBuffer
    {
        StagingBuffer(VmaAllocator& allocator, vk::BufferCreateInfo* buffer_info);
        ~StagingBuffer();

        template<typename T>
        T data() const
        {
            return reinterpret_cast<T>(m_alloc_info.pMappedData);
        }

        vk::Buffer buffer() const;

    private:
        VmaAllocator& m_allocator;
        VmaAllocationInfo m_alloc_info{};
        AllocatedBuffer m_buffer;
    };
} // namespace vkx
