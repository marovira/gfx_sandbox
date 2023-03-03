#include "staging_buffer.hpp"
#include "type_cast.hpp"

namespace vkx
{
    StagingBuffer::StagingBuffer(VmaAllocator& allocator,
                                 vk::BufferCreateInfo* buffer_info) :
        m_allocator{allocator}
    {
        VmaAllocationCreateInfo create_info{};
        create_info.usage = VMA_MEMORY_USAGE_AUTO;
        create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                            | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        if (vmaCreateBuffer(m_allocator,
                            to_vkc_ptr(buffer_info),
                            &create_info,
                            to_vkc_ptr(&m_buffer.buffer),
                            &m_buffer.allocation,
                            &m_alloc_info)
            != VK_SUCCESS)
        {
            throw std::runtime_error{"error: failed to allocate buffer"};
        }
    }

    StagingBuffer::~StagingBuffer()
    {
        vmaDestroyBuffer(m_allocator, m_buffer.buffer, m_buffer.allocation);
    }

    vk::Buffer StagingBuffer::buffer() const
    {
        return m_buffer.buffer;
    }

} // namespace vkx
