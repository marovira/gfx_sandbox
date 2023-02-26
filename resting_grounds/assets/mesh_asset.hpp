#pragma once

#include "asset_file.hpp"
#include "types.hpp"

namespace assets
{
    enum class VertexFormat
    {
        unknonw = 0,
        // Point-Normal-Colour-Texture-Tangent-BiTangent data in 32-bit float
        f32_pncvtb
    };

    struct Vertex
    {
        Vector3D<float> position;
        Vector3D<float> normal;
        Vector3D<float> colour;
        Vector2D<float> uv;
        Vector3D<float> tangent;
        Vector3D<float> bitangent;
    };

    struct MeshAsset
    {
        struct Bounds
        {
            Vector3D<float> origin;
            float radius;
            Vector3D<float> extents;
        };

        void read(AssetFile const& file);

        std::pair<std::vector<std::byte>, std::vector<std::byte>>
        unpack(std::vector<std::byte> const& source_buffer) const;

        AssetFile pack(std::vector<std::byte> const& vertex_data,
                       std::vector<std::byte> const& index_data) const;

        static Bounds calculate_bounds(std::vector<Vertex> const& vertices);

        std::uint64_t vertex_buffer_size;
        std::uint64_t index_buffer_size;
        Bounds bounds;
        VertexFormat vertex_format;
        std::uint8_t index_size;
        CompressionMode compression_mode;
        std::string original_file;
    };

} // namespace assets
