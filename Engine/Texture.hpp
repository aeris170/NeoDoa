#pragma once

#include <Engine/Graphics.hpp>

enum class TextureEncoding {
    PNG, BMP, TGA, JPG,
};

struct EncodedTextureData {
    std::string Name;
    RawData EncodedData;
    bool HasTransparency;
    TextureEncoding Encoding;
};

struct Texture {
    static const Texture& Missing() noexcept;

    std::string Name{};
    unsigned Width{ 1 }, Height{ 1 };
    unsigned Channels{};
    DataFormat Format{};
    RawData PixelData{};

    bool HasTransparency() const noexcept;

    EncodedTextureData Serialize(TextureEncoding encoding = TextureEncoding::PNG) const noexcept;
    static Texture Deserialize(const EncodedTextureData& data) noexcept;

    static Texture Copy(const Texture& texture) noexcept;
};
