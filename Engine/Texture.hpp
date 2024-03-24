#pragma once

#include "TypedefsAndConstants.hpp"
#include <Engine/Resolution.hpp>

enum class BaseInternalFormat {
    Red,
    RG,
    RGB,
    RGBA,
    DEPTH,
    DEPTH_STENCIL
};

enum class RFormat {
    R8,
    R16F,
    R32F
};
enum class RGFormat {
    RG8,
    RG16F,
    RG32F
};
enum class RGBFormat {
    RGB8,
    SRGB8,
    RGB16F,
    RGB32F,
    R11FG11FB10F
};
enum class RGBAFormat {
    RGBA8,
    RGBA12,
    RGBA16,
    SRGB8A8,
    RGBA16F,
    RGBA32F
};
enum class DepthFormat {
    DEPTH16,
    DEPTH24,
    DEPTH32,
    DEPTH32F
};
enum class DepthStencilFormat {
    DEPTH24_STENCIL8,
    DEPTH32F_STENCIL8
};

using ByteVector = std::vector<std::byte>;

enum class TextureEncoding {
    PNG, BMP, TGA, JPG,
};

struct EncodedTextureData {
    std::string Name;
    ByteVector Data;
    bool HasTransparency;
    TextureEncoding Encoding;
};

enum class TextureTransparency {
    YES, NO
};

struct Texture2D {
    static Texture2D CreateTextureR(RFormat format, Resolution resolution, ByteVector data = {}) noexcept;
    static Texture2D CreateTextureRG(RGFormat format, Resolution resolution, ByteVector data = {}) noexcept;
    static Texture2D CreateTextureRGB(RGBFormat format, Resolution resolution, ByteVector data = {}) noexcept;
    static Texture2D CreateTextureRGBA(RGBAFormat format, Resolution resolution, ByteVector data = {}) noexcept;
    static Texture2D CreateTextureDepth(DepthFormat format, Resolution resolution, ByteVector data = {}) noexcept;
    static Texture2D CreateTextureDepthStencil(DepthStencilFormat format, Resolution resolution, ByteVector data = {}) noexcept;

    GLuint ID;
    std::string Name{};
    size_t Width{}, Height{};
    unsigned Channels{};
    ByteVector PixelData{};

    EncodedTextureData Serialize(TextureEncoding encoding = TextureEncoding::PNG) const;
    static Texture2D Deserialize(const EncodedTextureData& data);

    static Texture2D Copy(const Texture2D& texture);
};

struct Texture {

    static const Texture& Empty() noexcept {
        static Texture empty{};
        return empty;
    };

    static const Texture& Missing() noexcept {
        static unsigned char data[] {
            0, 255, 255, 255,   0,   0,   0, 255,
            0,   0,   0, 255,   0, 255, 255, 255,
        };
        static Texture missing = Texture::CreateTextureRaw(
            "Missing Texture",
            data,
            2, 2,
            TextureTransparency::YES
            );
        return missing;
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
    bool operator==(const Texture& other) const noexcept;
    bool operator!=(const Texture& other) const noexcept;

    EncodedTextureData Serialize(TextureEncoding encoding = TextureEncoding::PNG) const;
    static Texture Deserialize(const EncodedTextureData& data);

    static Texture Copy(const Texture& texture);

private:

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
