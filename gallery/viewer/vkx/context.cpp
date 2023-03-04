#include "context.hpp"
#include "type_cast.hpp"
#include "vk_bootstrap.hpp"

#include <GLFW/glfw3.h>

#include <fmt/printf.h>
#include <zeus/assert.hpp>
#include <zeus/platform.hpp>

namespace vkx
{
    VKAPI_ATTR VkBool32 VKAPI_CALL
    debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                   VkDebugUtilsMessageTypeFlagsEXT type,
                   VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
                   void*)
    {
        auto ms = vkb::to_string_message_severity(severity);
        auto mt = vkb::to_string_message_type(type);
        fmt::print("[{} : {}]\n{}\n", ms, mt, callback_data->pMessage);

        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ASSERT(0);
        }

        return 0;
    }

    Context::Context(GLFWwindow* window) :
        m_vk_context{std::make_unique<vk::raii::Context>()}
    {
        vkb::InstanceBuilder builder;

        vkb::Instance inst = get_safe_vkb_result(builder
                                                     .require_api_version(1, 3, 0)
#if defined(ZEUS_BUILD_DEBUG)
                                                     .request_validation_layers(true)
                                                     .set_debug_callback(debug_callback)
#endif
                                                     .build());

        m_instance = std::make_unique<vk::raii::Instance>(*m_vk_context, inst.instance);
#if defined(ZEUS_BUILD_DEBUG)
        m_debug_messenger =
            std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*m_instance,
                                                               inst.debug_messenger);
#endif

        auto instance = to_vk_type(m_instance);

        VkSurfaceKHR surface;
        glfwCreateWindowSurface(instance, window, nullptr, &surface);
        m_surface = std::make_unique<vk::raii::SurfaceKHR>(*m_instance, surface);

        vkb::PhysicalDeviceSelector selector{inst};
        vkb::PhysicalDevice physical_device = get_safe_vkb_result(
            selector.set_minimum_version(1, 3).set_surface(surface).select());

        vkb::DeviceBuilder device_builder{physical_device};
        vk::PhysicalDeviceShaderDrawParameterFeatures shader_features{
            .shaderDrawParameters = true};

        vkb::Device device =
            get_safe_vkb_result(device_builder.add_pNext(&shader_features).build());

        vk::raii::PhysicalDevice tmp_device{*m_instance, device.physical_device};
        m_device = std::make_unique<vk::raii::Device>(tmp_device, device.device);

        m_active_device     = device.physical_device;
        m_device_properties = device.physical_device.properties;

        m_graphics_queue.queue =
            get_safe_vkb_result(device.get_queue(vkb::QueueType::graphics));
        m_graphics_queue.family_index =
            get_safe_vkb_result(device.get_queue_index(vkb::QueueType::graphics));

        VmaAllocatorCreateInfo alloc_info = {};
        alloc_info.physicalDevice         = m_active_device;
        alloc_info.device                 = to_vk_type(m_device);
        alloc_info.instance               = instance;

        m_allocator = std::make_unique<Allocator>(alloc_info);
    }

    Context::~Context()
    {
        m_allocator->free();
        m_allocator = nullptr;
    }

} // namespace vkx
