#include "prefab_asset.hpp"

#include <lz4.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace assets
{
    static constexpr auto sizeof_matrix = sizeof(Matrix4x4<float>);

    void PrefabAsset::read(AssetFile const& file)
    {
        auto metadata = nlohmann::json::parse(file.json);

        for (auto& [_, value] : metadata["node_matrices"].items())
        {
            node_matrices.insert({value[0], value[1]});
        }

        for (auto& [_, value] : metadata["node_names"].items())
        {
            node_names.insert({value[0], value[1]});
        }

        for (auto& [_, value] : metadata["node_parents"].items())
        {
            node_parents.insert({value[0], value[1]});
        }

        std::unordered_map<std::uint64_t, nlohmann::json> mesh_nodes =
            metadata["node_meshes"];

        for (auto pair : mesh_nodes)
        {
            NodeMesh node;

            node.mesh_path     = pair.second["mesh_path"];
            node.material_path = pair.second["material_path"];

            node_meshes.insert({pair.first, node});
        }

        std::size_t num_matrices = file.binary_blob.size() / sizeof_matrix;
        matrices.resize(num_matrices);

        std::memcpy(matrices.data(), file.binary_blob.data(), file.binary_blob.size());
    }

    AssetFile PrefabAsset::pack() const
    {
        nlohmann::json metadata;
        metadata["node_matrices"] = node_matrices;
        metadata["node_names"]    = node_names;
        metadata["node_parents"]  = node_parents;

        std::unordered_map<std::uint64_t, nlohmann::json> mesh_index;
        for (auto pair : node_meshes)
        {
            nlohmann::json mesh_node;
            mesh_node["mesh_path"]     = pair.second.mesh_path;
            mesh_node["material_path"] = pair.second.material_path;
            mesh_index.insert({pair.first, mesh_node});
        }

        metadata["node_meshes"] = mesh_index;

        AssetFile file;
        file.type    = {'P', 'R', 'F', 'B'};
        file.version = AssetFile::current_version;

        file.binary_blob.resize(matrices.size() * sizeof_matrix);
        std::memcpy(file.binary_blob.data(),
                    matrices.data(),
                    matrices.size() * sizeof_matrix);

        file.json = metadata.dump();
        return file;
    }
} // namespace assets
