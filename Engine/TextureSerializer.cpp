#include "TextureSerializer.hpp"

#include <cstring>
#include <cstdlib>

#include <stb_image_write.h>

SerializedTexture SerializeTexture(const Texture& texture) {
    SerializedTexture rv;
    rv.name = texture.Name();
    rv.hasTransparency = texture.HasTransparency();

    int texW = static_cast<int>(texture.Width());
    int texH = static_cast<int>(texture.Height());
    int stride = rv.hasTransparency ? 4 : 3;

    stbi_write_png_to_func(
        [](void* ctx, void* data, int size) {
            SerializedTexture* st = reinterpret_cast<SerializedTexture*>(ctx);
            st->data = std::malloc(size);
            assert(st->data != 0, "no memory?");
            std::memcpy(st->data, data, size);
        },
        &rv,
        texW,
        texH,
        stride,
        Texture::RequestPixelDataOf(texture).data(),
        texH * stride
    );

    return rv;
}
