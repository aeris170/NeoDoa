#include <Engine/Texture.hpp>

#include <Engine/TextureSerializer.hpp>
#include <Engine/TextureDeserializer.hpp>

const Texture& Texture::Missing() noexcept {
#define ND_BYTE(x) static_cast<std::byte>(x)
    static Texture missing{
        .Name = "Missing Texture",
        .Width = 2,
        .Height = 2,
        .Channels = 4,
        .Format = TextureFormat::RGBA8,
        .PixelData = {
            ND_BYTE(0), ND_BYTE(255), ND_BYTE(255), ND_BYTE(255), ND_BYTE(0), ND_BYTE(0)  , ND_BYTE(0)  , ND_BYTE(255),
            ND_BYTE(0), ND_BYTE(0)  , ND_BYTE(0)  , ND_BYTE(255), ND_BYTE(0), ND_BYTE(255), ND_BYTE(255), ND_BYTE(255)
        }
    };
#undef ND_BYTE
    return missing;
}

bool Texture::HasTransparency() const noexcept { return Channels == 4; }

EncodedTextureData Texture::Serialize(TextureEncoding encoding) const noexcept { return SerializeTexture(*this, encoding); }
Texture Texture::Deserialize(const EncodedTextureData& data) noexcept { return DeserializeTexture(data).deserializedTexture; }

Texture Texture::Copy(const Texture& texture) noexcept { return texture; }
