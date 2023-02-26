#pragma once

#include <core/io/input_stream.hpp>
#include <core/io/output_stream.hpp>

#include <array>
#include <string>
#include <vector>

namespace assets
{
    enum class CompressionMode
    {
        none = 0,
        lz4
    };

    struct AssetFile
    {
        static constexpr auto current_version{1};

        std::size_t size() const;

        void save(core::io::OutputStream& stream) const;
        void load(core::io::InputStream& stream);

        std::array<char, 4> type;
        std::uint32_t version;
        std::string json;
        std::vector<std::byte> binary_blob;
    };
} // namespace assets
