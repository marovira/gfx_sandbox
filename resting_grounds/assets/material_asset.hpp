#pragma once

#include "asset_file.hpp"

#include <unordered_map>

namespace assets
{
    enum class TransparencyMode
    {
        opaque,
        transparent,
    };

    struct MaterialAsset
    {
        void read(AssetFile const& file);
        AssetFile pack() const;

        std::string base_effect;
        std::unordered_map<std::string, std::string> textures;
        std::unordered_map<std::string, std::string> custom_properties;
        TransparencyMode transparency;
    };

} // namespace assets
