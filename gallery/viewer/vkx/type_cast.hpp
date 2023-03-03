#pragma once

#include "vulkan.hpp"

#include <type_traits>

namespace vkx
{
    // Convert from a pointer to a vk::raii object to the corresponding vk:: object.
    template<typename T>
    auto to_vk_type(T const& ptr)
    {
        return *(*ptr);
    }

    // Convert a vk:: type pointer to the pointer of the corresponding vk type
    // (C-version).
    template<typename T>
    auto to_vkc_ptr(T* ptr)
    {
        return reinterpret_cast<T::NativeType*>(ptr);
    }

    // Convert a vk:: flag type into the corresponding vk flag type (C-version).
    template<typename T>
    auto to_vkc_flag(T flag)
    {
        if constexpr (std::is_enum_v<T>)
        {
            using FlagType   = vk::Flags<T>;
            using VkFlagType = FlagType::MaskType;
            return static_cast<VkFlagType>(FlagType{flag});
        }
        else
        {
            using VkFlagType = T::MaskType;
            return static_cast<VkFlagType>(flag);
        }
    }
} // namespace vkx
