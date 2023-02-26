#pragma once

#include <filesystem>

namespace kass
{
    void konvert_file(std::filesystem::path const& file);
    void konvert_files(std::filesystem::path const& path, bool split);
} // namespace kass
