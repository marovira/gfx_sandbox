#pragma once

#include <assets/asset_file.hpp>

#include <optional>
#include <string>
#include <vector>

namespace kass
{
    bool is_valid_image(std::string const& filename);

    assets::AssetFile compress_image(std::string const& filename);
} // namespace kass
