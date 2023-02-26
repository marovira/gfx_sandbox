#include "konverter.hpp"
#include "konvert_image.hpp"

#include <core/io/file_output_stream.hpp>

#include <fmt/printf.h>

namespace fs = std::filesystem;

namespace kass
{
    void konvert_file(fs::path const& file)
    {
        if (file.extension() == ".gltf")
        {}
        else if (is_valid_image(file.string()))
        {
            auto c_file = compress_image(file.string());

            auto out = file.parent_path();
            out /= "image.kass";
            core::io::FileOutputStream stream{out.string()};
            c_file.save(stream);
        }
    }

    void konvert_files(std::filesystem::path const&, bool)
    {}
} // namespace kass

namespace detail
{} // namespace detail
