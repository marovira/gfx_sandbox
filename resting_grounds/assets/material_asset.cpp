#include "material_asset.hpp"

#include <fmt/printf.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace assets
{
    void MaterialAsset::read(AssetFile const& file)
    {
        auto material_metadata = nlohmann::json::parse(file.json);
        base_effect            = material_metadata["base_effect"];

        for (auto& [key, value] : material_metadata["textures"].items())
        {
            textures.insert({key, value});
        }

        for (auto& [key, value] : material_metadata["custom_properties"].items())
        {
            custom_properties.insert({key, value});
        }

        transparency = TransparencyMode::opaque;

        if (auto it = material_metadata.find("transparency");
            it != material_metadata.end())
        {
            std::string val = (*it);
            if (auto res = magic_enum::enum_cast<TransparencyMode>(val); res)
            {
                transparency = *res;
            }
            else
            {
                auto msg =
                    fmt::format("error: failed to parse transparency mode, recieved {}",
                                val);
                throw std::runtime_error{msg.c_str()};
            }
        }
    }

    AssetFile MaterialAsset::pack() const
    {
        nlohmann::json material_metadata;
        material_metadata["base_effect"]       = base_effect;
        material_metadata["textures"]          = textures;
        material_metadata["custom_properties"] = custom_properties;
        material_metadata["transparency"]      = magic_enum::enum_name(transparency);

        AssetFile file;
        file.type    = {'M', 'A', 'T', 'X'};
        file.version = AssetFile::current_version;
        file.json    = material_metadata.dump();

        return file;
    }
} // namespace assets
