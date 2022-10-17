#pragma once

#include "TypedefsAndConstants.hpp"

struct TextureData {

    std::string name;

    void* data;
    int dataSize;

    bool hasTransparency;
};

enum class TextureTransparency {
    YES, NO
};

struct Texture {

    using ByteVector = std::vector<unsigned char>;

    static Texture Empty() noexcept {
#ifdef _DEBUG
        Texture::FACTORY_FLAG = true; return { "", 0, 0, 0, TextureTransparency::NO }; Texture::FACTORY_FLAG = false;
#else
        return { "", 0, 0, 0, Transparency::NO };
#endif
    };

    static Texture CreateTexture(std::string_view name, const char* path, TextureTransparency transparency = TextureTransparency::YES);
    static Texture CreateTexture(std::string_view name, const unsigned char* data, size_t length, TextureTransparency transparency = TextureTransparency::YES);
    static Texture CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, TextureTransparency transparency = TextureTransparency::YES);

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

    TextureData Serialize() const;
    static Texture Deserialize(const TextureData& data);

    static Texture Copy(const Texture& scene);

private:
#ifdef _DEBUG
    static inline bool FACTORY_FLAG{ false };
#endif

    std::string _name{};
    size_t _width{ 0 }, _height{ 0 };
    TextureTransparency _transparency;
    TEX _glTextureID{ 0 };
    ByteVector _pixelData;

    /* don't call ctor, use factory functions */
    Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, TextureTransparency transparency) noexcept;

    void AllocateGPU() noexcept;
    void DeallocateGPU() noexcept;
};
