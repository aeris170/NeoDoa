#include <Engine/TextureSerializer.hpp>

#include <span>
#include <algorithm>

#include <stb_image_write.h>

static void WriteFunction(void* ctx, void* data, int size) {
    std::span pixels{ reinterpret_cast<const std::byte*>(data), static_cast<size_t>(size) };

    EncodedTextureData* etd = reinterpret_cast<EncodedTextureData*>(ctx);
    etd->EncodedData.reserve(size);

    std::ranges::copy(pixels, etd->EncodedData.begin());
}

EncodedTextureData SerializeTexture(const Texture& texture, TextureEncoding encoding) {
    stbi_flip_vertically_on_write(true);

    EncodedTextureData rv;
    rv.Name = texture.Name;
    rv.HasTransparency = texture.HasTransparency();

    int texW = static_cast<int>(texture.Width);
    int texH = static_cast<int>(texture.Height);
    int stride = texture.Channels;
    auto& pixel = texture.PixelData;

    switch (encoding) {
    using enum TextureEncoding;
    case PNG:
        stbi_write_png_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data(), texH * stride);
        break;
    case BMP:
        stbi_write_bmp_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data());
        break;
    case TGA:
        stbi_write_tga_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data());
        break;
    case JPG:
        stbi_write_jpg_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data(), 100);
        break;
    }

    rv.Encoding = encoding;
    return rv;
}
