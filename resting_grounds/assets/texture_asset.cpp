#include "texture_asset.hpp"

#include <core/memory_buffer.hpp>

#include <fmt/printf.h>
#include <lz4.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace assets
{
    void TextureAsset::read(AssetFile const& file)
    {
        auto metadata = nlohmann::json::parse(file.json);

        std::string format_string = metadata["format"];
        if (auto ret = magic_enum::enum_cast<TextureFormat>(format_string); ret)
        {
            texture_format = *ret;
        }
        else
        {
            auto msg = fmt::format("error: failed to parse texture format, received {}",
                                   format_string);
            throw std::runtime_error{msg.c_str()};
        }

        std::string compression_string = metadata["compression"];
        if (auto ret = magic_enum::enum_cast<CompressionMode>(compression_string); ret)
        {
            compression_mode = *ret;
        }
        else
        {
            auto msg = fmt::format("error: failed to parse compression mode, recieved {}",
                                   compression_string);
            throw std::runtime_error{msg.c_str()};
        }

        texture_size  = metadata["buffer_size"];
        original_file = metadata["original_file"];

        for (auto& [key, value] : metadata["pages"].items())
        {
            Page page;

            page.compressed_size = value["compressed_size"];
            page.original_size   = value["original_size"];
            page.width           = value["width"];
            page.height          = value["height"];

            pages.push_back(page);
        }
    }

    std::vector<std::byte>
    TextureAsset::unpack(std::vector<std::byte> const& source_buffer) const
    {
        // Loop through to figure out the size of the destination buffer.
        std::size_t dest_size = std::accumulate(pages.begin(),
                                                pages.end(),
                                                std::uint32_t{0},
                                                [](std::uint32_t sum, Page p) {
                                                    return sum + p.original_size;
                                                });

        std::vector<std::byte> destination(dest_size);
        auto source = core::to_const_data_ptr(source_buffer);
        auto dest   = core::to_data_ptr(destination);

        if (compression_mode == CompressionMode::lz4)
        {
            for (auto& page : pages)
            {
                if (LZ4_decompress_safe(source,
                                        dest,
                                        page.compressed_size,
                                        page.original_size)
                    < 0)
                {
                    return {};
                }
                source += page.compressed_size;
                dest += page.original_size;
            }
        }
        else
        {
            destination = source_buffer;
        }

        return destination;
    }

    std::vector<std::byte>
    TextureAsset::unpack_page(int page_index,
                              std::vector<std::byte> const& source_buffer) const
    {
        auto source = core::to_const_data_ptr(source_buffer);
        for (int i{0}; i < page_index; ++i)
        {
            source += pages[i].compressed_size;
        }

        auto page = pages[page_index];
        std::vector<std::byte> destination(page.original_size);

        if (compression_mode == CompressionMode::lz4)
        {
            // Check if the compressed size matches the original. If it does, it wasn't
            // compressed to begin with.
            if (page.compressed_size != page.original_size)
            {
                if (LZ4_decompress_safe(source,
                                        core::to_data_ptr(destination),
                                        page.compressed_size,
                                        page.original_size)
                    < 0)
                {
                    return {};
                }
            }
            else
            {
                std::memcpy(core::to_data_ptr(destination), source, page.original_size);
            }
        }
        else
        {
            std::memcpy(core::to_data_ptr(destination), source, page.original_size);
        }

        return destination;
    }

    AssetFile TextureAsset::pack(std::vector<std::byte> const& pixel_data)
    {
        AssetFile file;
        file.type    = {'T', 'E', 'X', 'I'};
        file.version = AssetFile::current_version;

        auto pixels = core::to_const_data_ptr(pixel_data);
        std::vector<std::byte> page_buffer;
        for (auto& p : pages)
        {
            page_buffer.resize(p.original_size);
            int compressed_size{0};
            if (compression_mode == CompressionMode::lz4)
            {
                int compress_staging = LZ4_compressBound(p.original_size);
                page_buffer.resize(compress_staging);
                compressed_size = LZ4_compress_default(pixels,
                                                       core::to_data_ptr(page_buffer),
                                                       p.original_size,
                                                       compress_staging);

                float compression_rate =
                    compressed_size / static_cast<float>(texture_size);

                if (compression_rate > 0.8f)
                {
                    compressed_size = p.original_size;
                    page_buffer.resize(compressed_size);
                    std::memcpy(page_buffer.data(), pixels, compressed_size);
                }
                else
                {
                    page_buffer.resize(compressed_size);
                }
            }
            else
            {
                compressed_size = p.original_size;
                page_buffer.resize(compressed_size);
                std::memcpy(page_buffer.data(), pixels, compressed_size);
            }

            p.compressed_size = compressed_size;
            file.binary_blob.insert(file.binary_blob.end(),
                                    page_buffer.begin(),
                                    page_buffer.end());

            pixels += p.original_size;
        }

        nlohmann::json metadata;
        metadata["format"]        = magic_enum::enum_name(texture_format);
        metadata["buffer_size"]   = texture_size;
        metadata["original_file"] = original_file;
        metadata["compression"]   = magic_enum::enum_name(compression_mode);

        std::vector<nlohmann::json> page_json;
        for (auto& p : pages)
        {
            nlohmann::json page;
            page["compressed_size"] = p.compressed_size;
            page["original_size"]   = p.original_size;
            page["width"]           = p.width;
            page["height"]          = p.height;
            page_json.push_back(page);
        }
        metadata["pages"] = page_json;

        file.json = metadata.dump();
        return file;
    }
} // namespace assets
