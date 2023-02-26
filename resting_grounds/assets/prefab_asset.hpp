#pragma once

#include "asset_file.hpp"
#include "types.hpp"

#include <unordered_map>

namespace assets
{
    struct PrefabAsset
    {
        void read(AssetFile const& file);
        AssetFile pack() const;

        struct NodeMesh
        {
            std::string material_path;
            std::string mesh_path;
        };

        std::unordered_map<std::uint64_t, int> node_matrices;
        std::unordered_map<std::uint64_t, std::string> node_names;
        std::unordered_map<std::uint64_t, std::uint64_t> node_parents;
        std::unordered_map<std::uint64_t, NodeMesh> node_meshes;
        std::vector<Matrix4x4<float>> matrices;
    };
} // namespace assets
