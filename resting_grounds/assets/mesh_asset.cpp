#include "mesh_asset.hpp"

#include <core/memory_buffer.hpp>

#include <fmt/printf.h>
#include <lz4.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <limits>

namespace assets
{
    void MeshAsset::read(AssetFile const& file)
    {
        auto metadata = nlohmann::json::parse(file.json);

        vertex_buffer_size = metadata["vertex_buffer_size"];
        index_buffer_size  = metadata["index_buffer_size"];
        index_size         = static_cast<std::uint8_t>(metadata["index_size"]);
        original_file      = metadata["original_file"];

        std::string mode = metadata["compression_mode"];
        if (auto res = magic_enum::enum_cast<CompressionMode>(mode); res)
        {
            compression_mode = *res;
        }
        else
        {
            auto msg =
                fmt::format("error: failed parsing compression mode, got {}", mode);
            throw std::runtime_error{msg.c_str()};
        }

        std::vector<float> bounds_data(7);
        bounds_data = metadata["bounds"].get<std::vector<float>>();

        bounds.origin[0] = bounds_data[0];
        bounds.origin[1] = bounds_data[1];
        bounds.origin[2] = bounds_data[2];

        bounds.radius = bounds_data[3];

        bounds.extents[0] = bounds_data[4];
        bounds.extents[1] = bounds_data[5];
        bounds.extents[2] = bounds_data[6];

        std::string format = metadata["vertex_format"];
        if (auto res = magic_enum::enum_cast<VertexFormat>(format); res)
        {
            vertex_format = *res;
        }
        else
        {
            auto msg = fmt::format("error: failed parsing vertex format, got {}", format);
            throw std::runtime_error{msg.c_str()};
        }
    }

    std::pair<std::vector<std::byte>, std::vector<std::byte>>
    MeshAsset::unpack(std::vector<std::byte> const& source_buffer) const
    {
        std::vector<std::byte> decompressed_buffer;
        decompressed_buffer.resize(vertex_buffer_size + index_buffer_size);

        if (LZ4_decompress_safe(core::to_const_data_ptr(source_buffer),
                                core::to_data_ptr(decompressed_buffer),
                                static_cast<int>(decompressed_buffer.size()),
                                static_cast<int>(decompressed_buffer.size()))
            < 0)
        {
            throw std::runtime_error{"error: failed de-compressing mesh buffer"};
        }

        std::vector<std::byte> vertex_buffer(vertex_buffer_size);
        std::memcpy(vertex_buffer.data(),
                    decompressed_buffer.data(),
                    vertex_buffer.size());

        std::vector<std::byte> index_buffer(index_buffer_size);
        std::memcpy(index_buffer.data(),
                    decompressed_buffer.data() + vertex_buffer.size(),
                    index_buffer.size());

        return std::pair{vertex_buffer, index_buffer};
    }

    AssetFile MeshAsset::pack(std::vector<std::byte> const& vertex_data,
                              std::vector<std::byte> const& index_data) const
    {
        AssetFile file;
        file.type    = {'M', 'E', 'S', 'H'};
        file.version = AssetFile::current_version;

        nlohmann::json metadata;
        metadata["vertex_format"]      = magic_enum::enum_name(vertex_format);
        metadata["vertex_buffer_size"] = vertex_buffer_size;
        metadata["index_buffer_size"]  = index_buffer_size;
        metadata["index_size"]         = index_size;
        metadata["original_file"]      = original_file;

        std::vector<float> bounds_data(7);
        bounds_data[0] = bounds.origin[0];
        bounds_data[1] = bounds.origin[1];
        bounds_data[2] = bounds.origin[2];

        bounds_data[3] = bounds.radius;

        bounds_data[4] = bounds.extents[0];
        bounds_data[5] = bounds.extents[1];
        bounds_data[6] = bounds.extents[2];

        metadata["bounds"] = bounds_data;

        std::size_t full_size = vertex_buffer_size + index_buffer_size;

        std::vector<std::byte> merged_buffer(full_size);
        std::copy(vertex_data.begin(), vertex_data.end(), merged_buffer.begin());
        std::copy(index_data.begin(),
                  index_data.end(),
                  merged_buffer.begin() + vertex_buffer_size);

        std::size_t compress_staging = LZ4_compressBound(static_cast<int>(full_size));
        file.binary_blob.resize(compress_staging);

        int compressed_size = LZ4_compress_default(core::to_const_data_ptr(merged_buffer),
                                                   core::to_data_ptr(file.binary_blob),
                                                   static_cast<int>(merged_buffer.size()),
                                                   static_cast<int>(compress_staging));
        file.binary_blob.resize(compressed_size);

        metadata["compression"] = "lz4";

        file.json = metadata.dump();
        return file;
    }

    MeshAsset::Bounds MeshAsset::calculate_bounds(std::vector<Vertex> const& vertices)
    {
        Vector3D<float> min;
        min.fill(std::numeric_limits<float>::max());

        Vector3D<float> max;
        max.fill(std::numeric_limits<float>::min());

        for (auto v : vertices)
        {
            min[0] = std::min(min[0], v.position[0]);
            min[1] = std::min(min[1], v.position[1]);
            min[2] = std::min(min[2], v.position[2]);

            max[0] = std::max(max[0], v.position[0]);
            max[1] = std::max(max[1], v.position[1]);
            max[2] = std::max(max[2], v.position[2]);
        }

        Bounds bounds;
        bounds.extents[0] = (max[0] - min[0]) / 2.0f;
        bounds.extents[1] = (max[1] - min[1]) / 2.0f;
        bounds.extents[2] = (max[2] - min[2]) / 2.0f;

        bounds.origin[0] = bounds.extents[0] + min[0];
        bounds.origin[1] = bounds.extents[1] + min[1];
        bounds.origin[2] = bounds.extents[2] + min[2];

        float r2{0.0f};
        for (auto v : vertices)
        {
            Vector3D<float> offset;
            offset[0] = v.position[0] - bounds.origin[0];
            offset[1] = v.position[1] - bounds.origin[1];
            offset[2] = v.position[2] - bounds.origin[2];

            float distance =
                std::inner_product(offset.begin(), offset.end(), offset.begin(), 0.0f);
            r2 = std::max(r2, distance);
        }

        bounds.radius = std::sqrt(r2);

        return bounds;
    }

} // namespace assets
