#pragma once

#include "TypedefsAndConstants.hpp"

struct Texture {

    using ByteVector = std::vector<unsigned char>;

    enum class Transparency {
        YES, NO
    };

    static Texture Empty() noexcept {
#ifdef _DEBUG
        Texture::FACTORY_FLAG = true; return { "", 0, 0, 0, Transparency::NO }; Texture::FACTORY_FLAG = false;
#else
        return { "", 0, 0, 0, Transparency::NO };
#endif
    };

    static Texture CreateTexture(std::string_view name, const char* path, Transparency transparency = Transparency::YES);
    static Texture CreateTexture(std::string_view name, const unsigned char* data, size_t length, Transparency transparency = Transparency::YES);
    static Texture CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, Transparency transparency = Transparency::YES);

    static ByteVector RequestPixelDataOf(const Texture& texture);
    static void ApplyPixelDataTo(Texture& texture, const ByteVector& data);
    static void ApplyPixelDataTo(Texture& texture, ByteVector&& data);

    const std::string& Name() const;
    size_t Width() const;
    size_t Height() const;
    bool HasTransparency() const;
    TEX TextureID() const;

    void Bind(int slot = 0);

    ~Texture() noexcept;
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) noexcept;
    bool operator==(const Texture& other) noexcept;
    bool operator!=(const Texture& other) noexcept;

    static Texture Copy(const Texture& scene);

private:
#ifdef _DEBUG
    static inline bool FACTORY_FLAG{ false };
#endif

    std::string _name{};
    size_t _width{ 0 }, _height{ 0 };
    Transparency _transparency;
    TEX _glTextureID{ 0 };
    ByteVector _pixelData;

    /* don't call ctor, use factory functions */
    Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, Transparency transparency) noexcept;

    void AllocateGPU() noexcept;
    void DeallocateGPU() noexcept;
};
