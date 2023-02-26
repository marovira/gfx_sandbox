#include "konvert_image.hpp"

#include <assets/texture_asset.hpp>
#include <core/memory_buffer.hpp>

#include <fmt/printf.h>
#include <stb_image.h>
#if defined(KASS_USE_NVTT)
#    include <nvtt/nvtt.h>
#else
#    include <stb_image_resize.h>
#endif

namespace kass
{
    bool is_valid_image(std::string const& filename)
    {
        int w, h, c;
        return stbi_info(filename.c_str(), &w, &h, &c) == 1;
    }

#if defined(KASS_USE_NVTT)
    std::vector<std::byte> compress_nvtt(assets::TextureAsset& texture,
                                         int width,
                                         int height,
                                         void const* pixels,
                                         bool is_hdr)
    {
        using assets::TextureAsset;
        struct OutHandler : nvtt::OutputHandler
        {
            bool writeData(void const* data, int size) override
            {
                // Expand buffer and copy data in.
                std::size_t cur_size = buffer.size();
                buffer.resize(cur_size + size);

                auto ptr = core::to_data_ptr(buffer) + cur_size;
                std::memcpy(ptr, data, size);
                return true;
            }

            void beginImage(int, int, int, int, int, int) override
            {}

            void endImage() override
            {}

            std::vector<std::byte> buffer;
        };

        nvtt::Context context;
        nvtt::CompressionOptions compress_options;
        compress_options.setFormat(is_hdr ? nvtt::Format_BC6S : nvtt::Format_BC7);

        nvtt::OutputOptions out_options;
        OutHandler handler;
        out_options.setOutputHandler(&handler);

        nvtt::Surface image;
        image.setImage(is_hdr ? nvtt::InputFormat_RGBA_32F : nvtt::InputFormat_BGRA_8UB,
                       width,
                       height,
                       1,
                       pixels);

        std::vector<std::byte> compressed_data;
        const int num_mips = image.countMipmaps();
        for (int mip{0}; mip < num_mips; ++mip)
        {
            if (!context.compress(image, 0, mip, compress_options, out_options))
            {
                fmt::print("error: compression failed");
                return {};
            }

            if (mip == num_mips - 1)
            {
                break;
            }

            TextureAsset::Page page;
            page.width         = image.width();
            page.height        = image.height();
            page.original_size = static_cast<std::uint32_t>(handler.buffer.size());
            texture.pages.push_back(page);

            compressed_data.insert(compressed_data.end(),
                                   handler.buffer.begin(),
                                   handler.buffer.end());
            handler.buffer.clear();

            if (is_hdr)
            {
                image.toLinear(2.2f);
            }
            else
            {
                image.toLinearFromSrgb();
            }

            image.premultiplyAlpha();
            image.buildNextMipmap(nvtt::MipmapFilter_Box);
            image.demultiplyAlpha();
            image.toSrgb();
        }

        return compressed_data;
    }
#else

    int count_mipmaps(int width, int height)
    {
        int count{1};
        while (width > 1 && height > 1)
        {
            width  = width / 2;
            height = height / 2;
            ++count;
        }

        return count + 1;
    }

    std::vector<std::byte> compress_regular(assets::TextureAsset& texture,
                                            int width,
                                            int height,
                                            void const* pixels,
                                            bool is_hdr)
    {
        using assets::TextureAsset;

        const int num_mips = count_mipmaps(width, height);
        int mip_w{width};
        int mip_h{height};
        std::vector<std::byte> compressed_data;
        std::size_t data_size = (is_hdr) ? sizeof(float) : sizeof(std::uint8_t);
        for (int mip{0}; mip < num_mips; ++mip)
        {
            std::vector<std::byte> mip_bytes(mip_w * mip_h * data_size * 4);
            int ret{0};
            if (is_hdr)
            {
                ret = stbir_resize_float(reinterpret_cast<const float*>(pixels),
                                         width,
                                         height,
                                         0,
                                         reinterpret_cast<float*>(mip_bytes.data()),
                                         mip_w,
                                         mip_h,
                                         0,
                                         4);
            }
            else
            {
                ret =
                    stbir_resize_uint8(reinterpret_cast<const unsigned char*>(pixels),
                                       width,
                                       height,
                                       0,
                                       reinterpret_cast<unsigned char*>(mip_bytes.data()),
                                       mip_w,
                                       mip_h,
                                       0,
                                       4);
            }

            if (!ret)
            {
                fmt::print("error: compression failed");
                return {};
            }

            if (mip == num_mips - 1)
            {
                break;
            }

            TextureAsset::Page page;
            page.width         = mip_w;
            page.height        = mip_h;
            page.original_size = static_cast<std::uint32_t>(mip_bytes.size());
            texture.pages.push_back(page);

            compressed_data.insert(compressed_data.end(),
                                   mip_bytes.begin(),
                                   mip_bytes.end());
        }

        return compressed_data;
    }
#endif

    assets::AssetFile compress_image(std::string const& filename)
    {
        using assets::TextureAsset;
        using assets::TextureFormat;

        int width, height, channels;
        void* pixels{nullptr};
        bool is_hdr{false};
        if (stbi_is_hdr(filename.c_str()))
        {
            pixels = stbi_loadf(filename.c_str(), &width, &height, &channels, 4);
            is_hdr = true;
        }
        else
        {
            pixels = stbi_load(filename.c_str(), &width, &height, &channels, 4);
        }

        if (!pixels)
        {
            fmt::print("error: unable to open file {}", filename);
            return {};
        }

        int texture_size = width * height * channels;

        TextureAsset texture;
        texture.texture_size = texture_size;
        texture.texture_format =
            (is_hdr) ? TextureFormat::rgba_float32 : TextureFormat::rgba_uint8;
        texture.original_file = filename;

#if defined(KASS_USE_NVTT)
        auto bytes = compress_nvtt(texture, width, height, pixels, is_hdr);
#else
        auto bytes = compress_regular(texture, width, height, pixels, is_hdr);
#endif
        if (bytes.empty())
        {
            return {};
        }

        texture.texture_size = bytes.size();
        stbi_image_free(pixels);

        return texture.pack(bytes);
    }
} // namespace kass
