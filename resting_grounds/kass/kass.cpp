#include "konverter.hpp"

#include <argparse/argparse.hpp>
#include <fmt/printf.h>

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct Options
{
    std::vector<fs::path> input_paths;
    fs::path output_path;
    bool split{false};
};

std::pair<Options, int> parse_args(int argc, char* argv[])
{
    argparse::ArgumentParser parser{"kass"};
    parser.add_description("A travelling konverter for asset files");

    parser.add_argument("files")
        .metavar("FILES")
        .nargs(argparse::nargs_pattern::at_least_one)
        .help("A list of files or folders containing assets to be converted");
    parser.add_argument("-s", "--split")
        .default_value(false)
        .implicit_value(true)
        .help("Convert files individually, otherwise bundle into a single file per root "
              "directory");
    parser.add_argument("-o", "--out")
        .metavar("OUT")
        .nargs(1)
        .help("Output directory where the converted files will be placed");
    parser.add_epilog(
        "This tool converts GLTF and all related files into a pre - processed asset file "
        "to speed up\n"
        "loading times of large assets. If the inputs contain directories, then these "
        "will be\n"
        "scanned recursively. By default, kass will convert each file individually, "
        "including\n"
        "splitting up meshes that are contained in a single file unless -s is specified, "
        "in which\n"
        "case they are merged into a single file per root directory (if the original "
        "input was a\n"
        "directory). Converted files are saved next to the source files unless -o is "
        "provided\n");

    Options opt;

    try
    {
        parser.parse_args(argc, argv);
    }
    catch (std::runtime_error const& e)
    {
        fmt::print("{}\n", e.what());
        fmt::print("{}\n", parser.help().str());
        return {opt, -1};
    }

    auto inputs = parser.get<std::vector<std::string>>("files");
    for (auto& input : inputs)
    {
        fs::path path{input};
        if (!fs::exists(path))
        {
            fmt::print("error: {} does not exist", path.string());
            return {opt, -1};
        }

        opt.input_paths.push_back(fs::absolute(path));
    }

    if (auto arg = parser.present("-o"); arg)
    {
        opt.output_path = fs::path{*arg};
    }

    if (parser["-s"] == true)
    {
        opt.split = true;
    }

    return {opt, 0};
}

int main(int argc, char** argv)
{
    auto [opt, ret] = parse_args(argc, argv);
    if (ret != 0)
    {
        return ret;
    }

    for (auto path : opt.input_paths)
    {
        kass::konvert_file(path);
    }

    return 0;
}
