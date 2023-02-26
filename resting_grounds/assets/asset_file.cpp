#include "asset_file.hpp"

#include <fmt/printf.h>

namespace assets
{
    std::size_t AssetFile::size() const
    {
        std::size_t size{0};
        size += 4;                     // type.
        size += sizeof(std::uint32_t); // version.
        size += json.size();           // JSON metadata.
        size += binary_blob.size();    // Binary data.
        return size;
    }

    void AssetFile::save(core::io::OutputStream& stream) const
    {
        stream.write_four_cc(type);
        stream.write_value(version);
        stream.write_buffer(json);
        stream.write_buffer(binary_blob);
    }

    void AssetFile::load(core::io::InputStream& stream)
    {
        type    = stream.read_four_cc();
        version = stream.read_value<std::uint32_t>();

        if (version != current_version)
        {
            std::string msg = fmt::format("error: incompatible version found, expected "
                                          "version {} but got version {}",
                                          current_version,
                                          version);
            throw std::runtime_error{msg.c_str()};
        }

        json        = stream.read_buffer<std::string>();
        binary_blob = stream.read_buffer<std::vector<std::byte>>();
    }
} // namespace assets
