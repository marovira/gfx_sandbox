#pragma once

#include "asset_file.hpp"

namespace assets
{
    enum class TextureFormat
    {
        unknonw = 0,
        rgba_uint8,
        rgba_float32
    };

    struct TextureAsset
    {
        void read(AssetFile const& file);
        std::vector<std::byte> unpack(std::vector<std::byte> const& source_buffer) const;
        std::vector<std::byte>
        unpack_page(int page_index, std::vector<std::byte> const& source_buffer) const;

        AssetFile pack(std::vector<std::byte> const& pixel_data);

        struct Page
        {
            std::uint32_t width;
            std::uint32_t height;
            std::uint32_t compressed_size;
            std::uint32_t original_size;
        };

        std::uint64_t texture_size;
        TextureFormat texture_format;
        CompressionMode compression_mode;

        std::string original_file;
        std::vector<Page> pages;
    };
} // namespace assets
