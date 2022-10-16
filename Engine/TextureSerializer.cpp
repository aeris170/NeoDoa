#include "TextureSerializer.hpp"

#include <cstring>
#include <cstdlib>

#include <stb_image_write.h>

SerializedTexture SerializeTexture(const Texture& texture) {
    SerializedTexture rv;
    rv.name = texture.Name();
    rv.hasTransparency = texture.HasTransparency();
    int stride = rv.hasTransparency ? 4 : 3;

    stbi_write_png_to_func([](void* ctx, void* data, int size) {
        SerializedTexture* st = reinterpret_cast<SerializedTexture*>(ctx);
        st->data = std::malloc(size);
        std::memcpy(st->data, data, size);
    }, &rv, texture.Width(), texture.Height(), stride, Texture::RequestPixelDataOf(texture).data(), texture.Width() * stride);

    return rv;
}
