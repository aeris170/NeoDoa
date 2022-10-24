#pragma once

#include "TypedefsAndConstants.hpp"

using ByteVector = std::vector<std::byte>;

enum class TextureEncoding {
    PNG, BMP, TGA, JPG,
};

struct EncodedTextureData {

    std::string name;

    ByteVector data;

    bool hasTransparency;

    TextureEncoding encoding;
};

enum class TextureTransparency {
    YES, NO
};

struct Texture {

    static Texture Empty() noexcept {
#ifdef _DEBUG
        Texture::FACTORY_FLAG = true; return {}; Texture::FACTORY_FLAG = false;
#else
        return {};
#endif
    };

    static Texture CreateTexture(std::string_view name, const char* path, TextureTransparency transparency = TextureTransparency::YES);
    static Texture CreateTexture(std::string_view name, const unsigned char* data, size_t length, TextureTransparency transparency = TextureTransparency::YES);
    static Texture CreateTexture(std::string_view name, ByteVector data, TextureTransparency transparency = TextureTransparency::YES);
    static Texture CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, TextureTransparency transparency = TextureTransparency::YES);

    static const std::byte* const GetByteBufferOf(const Texture& texture);
    static ByteVector RequestPixelDataOf(const Texture& texture);
    static void ApplyPixelDataTo(Texture& texture, const ByteVector& data);
    static void ApplyPixelDataTo(Texture& texture, ByteVector&& data);

    void Bind(int slot = 0);

    const std::string& Name() const;
    size_t Width() const;
    size_t Height() const;
    bool HasTransparency() const;
    TEX TextureID() const;
    void* TextureIDRaw() const;

    ~Texture() noexcept;
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) noexcept;
    bool operator==(const Texture& other) noexcept;
    bool operator!=(const Texture& other) noexcept;

    EncodedTextureData Serialize(TextureEncoding encoding = TextureEncoding::PNG) const;
    static Texture Deserialize(const EncodedTextureData& data);

    static Texture Copy(const Texture& scene);

private:
#ifdef _DEBUG
    static inline bool FACTORY_FLAG{ false };
#endif

    std::string _name{};
    size_t _width{ 0 }, _height{ 0 };
    TextureTransparency _transparency{ TextureTransparency::NO };
    TEX _glTextureID{ 0 };
    ByteVector _pixelData{};

    /* don't call ctor, use factory functions */
    Texture() noexcept;
    Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, TextureTransparency transparency) noexcept;

    void AllocateGPU() noexcept;
    void DeallocateGPU() noexcept;
};
