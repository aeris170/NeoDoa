#include "TextureSerializer.hpp"

#include <cstring>
#include <cstdlib>
#include <span>

#include <stb_image_write.h>

static void WriteFunction(void* ctx, void* data, int size) {
    std::span pixels{ reinterpret_cast<std::byte*>(data), static_cast<size_t>(size) };

    EncodedTextureData* etd = reinterpret_cast<EncodedTextureData*>(ctx);
    etd->Data.reserve(size);

    std::ranges::copy(pixels, etd->Data.begin());
}

EncodedTextureData SerializeTexture(const Texture& texture, TextureEncoding encoding) {
    stbi_flip_vertically_on_write(true);

    EncodedTextureData rv;
    rv.Name = texture.Name();
    rv.HasTransparency = texture.HasTransparency();

    int texW = static_cast<int>(texture.Width());
    int texH = static_cast<int>(texture.Height());
    int stride = rv.HasTransparency ? 4 : 3;
    auto pixel = Texture::RequestPixelDataOf(texture);

    switch (encoding) {
    case TextureEncoding::PNG:
        stbi_write_png_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data(), texH * stride);
        break;
    case TextureEncoding::BMP:
        stbi_write_bmp_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data());
        break;
    case TextureEncoding::TGA:
        stbi_write_tga_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data());
        break;
    case TextureEncoding::JPG:
        stbi_write_jpg_to_func(WriteFunction, &rv, texW, texH, stride, pixel.data(), 100);
        break;
    }

    rv.Encoding = encoding;
    return rv;
}
