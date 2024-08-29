#include <Engine/Texture.hpp>

#include <Engine/TextureSerializer.hpp>
#include <Engine/TextureDeserializer.hpp>

const Texture& Texture::Missing() noexcept {
#define ND_BYTE(x) static_cast<std::byte>(x)

    constexpr unsigned textureSize = 256;
    static bool initialized = false;
    static std::vector<std::byte> pixelData(textureSize * textureSize * 4);

    if (!initialized) {
        // Fill the textureSize x textureSize texture with the same pattern as the original 2x2 texture
        for (unsigned y = 0; y < textureSize; ++y) {
            for (unsigned x = 0; x < textureSize; ++x) {
                unsigned index = (y * textureSize + x) * 4;
                bool isCyan = ((x / (textureSize / 2)) % 2) == ((y / (textureSize / 2)) % 2);
                if (isCyan) {
                    pixelData[index] = ND_BYTE(0);
                    pixelData[index + 1] = ND_BYTE(255);
                    pixelData[index + 2] = ND_BYTE(255);
                    pixelData[index + 3] = ND_BYTE(255);
                } else {
                    pixelData[index] = ND_BYTE(0);
                    pixelData[index + 1] = ND_BYTE(0);
                    pixelData[index + 2] = ND_BYTE(0);
                    pixelData[index + 3] = ND_BYTE(255);
                }
            }
        }
        initialized = true;
    }

    static Texture missing{
        .Name = "Missing Texture",
        .Width = textureSize,
        .Height = textureSize,
        .Channels = 4,
        .Format = DataFormat::RGBA8,
        .PixelData = pixelData
    };

#undef ND_BYTE
    return missing;
}

bool Texture::HasTransparency() const noexcept { return Channels == 4; }

EncodedTextureData Texture::Serialize(TextureEncoding encoding) const noexcept { return SerializeTexture(*this, encoding); }
Texture Texture::Deserialize(const EncodedTextureData& data) noexcept { return DeserializeTexture(data).deserializedTexture; }

Texture Texture::Copy(const Texture& texture) noexcept { return texture; }

