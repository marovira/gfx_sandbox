#pragma once

#include <array>

namespace assets
{
    template<typename T>
    using Vector3D = std::array<T, 3>;

    template<typename T>
    using Vector2D = std::array<T, 2>;

    template<typename T>
    using Matrix4x4 = std::array<T, 16>;

} // namespace assets
