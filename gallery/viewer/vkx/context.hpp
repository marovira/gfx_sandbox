#pragma once

#include "types.hpp"
#include "vulkan.hpp"

#include <vk_mem_alloc.h>

#include <functional>
#include <memory>

struct GLFWwindow;

namespace vkx
{
    class Context
    {
    public:
        Context(GLFWwindow* window);

    private:
        std::unique_ptr<vk::raii::Context> m_vk_context;
        std::unique_ptr<vk::raii::Instance> m_instance;
        std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> m_debug_messenger;

        std::unique_ptr<vk::raii::SurfaceKHR> m_surface;
        std::unique_ptr<vk::raii::Device> m_device;
        vk::PhysicalDevice m_active_device;
        vk::PhysicalDeviceProperties m_device_properties;

        Queue m_graphics_queue;
        VmaAllocator m_allocator{nullptr};
    };
} // namespace vkx
