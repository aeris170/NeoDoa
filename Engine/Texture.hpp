#pragma once

#include "TypedefsAndConstants.hpp"

struct Texture {
    static Texture Empty() { return{ "", 0, 0, 0, false }; };

    static Texture CreateTexture(std::string_view name, const char* path, bool hasTransparency = true);
    static Texture CreateTexture(std::string_view name, const unsigned char* data, size_t length, bool hasTransparency = true);
    static Texture CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, bool hasTransparency = true);

    const std::string& Name() const;
    TEX TextureID() const;

    void Bind(int slot = 0);

    ~Texture() noexcept;
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) noexcept;
    bool operator==(const Texture& other);
    bool operator!=(const Texture& other);

private:
#ifdef _DEBUG
    static inline bool FACTORY_FLAG{ false };
#endif

    using ByteVector = std::vector<unsigned char>;

    std::string _name;
    TEX _glTextureID{ 0 };
    ByteVector _pixelData;

    /* don't call ctor, use factory functions */
    Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, bool hasTransparency) noexcept;
};
