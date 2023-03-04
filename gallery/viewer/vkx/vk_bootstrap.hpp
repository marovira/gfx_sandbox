#pragma once

#include <fmt/printf.h>
#include <VkBootstrap.h>

namespace vkx
{
    template<typename T>
    T get_safe_vkb_result(vkb::detail::Result<T> const& ret)
    {
        if (ret)
        {
            return *ret;
        }

        auto code = ret.error();
        auto msg  = fmt::format("({}): {}", code.value(), code.message());
        throw std::runtime_error{msg.c_str()};
    }
} // namespace vkx
