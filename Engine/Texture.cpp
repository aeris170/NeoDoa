#include "Texture.hpp"

#include <unordered_map>

#include <stb_image.h>

#include <Utility/TemplateUtilities.hpp>

#include "TextureSerializer.hpp"
#include "TextureDeserializer.hpp"
#include "Log.hpp"

static auto BaseToGL(BaseInternalFormat base) {
    switch (base) {
    using enum BaseInternalFormat;
    case Red:           return GL_RED;
    case RG:            return GL_RG;
    case RGB:           return GL_RGB;
    case RGBA:          return GL_RGBA;
    case DEPTH:         return GL_DEPTH_COMPONENT;
    case DEPTH_STENCIL: return GL_DEPTH_STENCIL;
    }
}

static auto FormatToGL(RFormat format) {
    switch (format) {
    using enum RFormat;
    case R8:           return GL_R8;
    case R16F:         return GL_R16F;
    case R32F:         return GL_R32F;
    }
}
static auto FormatToGL(RGFormat format) {
    switch (format) {
    using enum RGFormat;
    case RG8:          return GL_RG8;
    case RG16F:        return GL_RG16F;
    case RG32F:        return GL_RG32F;
    }
}
static auto FormatToGL(RGBFormat format) {
    switch (format) {
    using enum RGBFormat;
    case RGB8:         return GL_RGB8;
    case SRGB8:        return GL_SRGB8;
    case RGB16F:       return GL_RGB16F;
    case RGB32F:       return GL_RGB32F;
    case R11FG11FB10F: return GL_R11F_G11F_B10F;
    }
}
static auto FormatToGL(RGBAFormat format) {
    switch (format) {
    using enum RGBAFormat;
    case RGBA8:   return GL_RGBA8;
    case RGBA12:  return GL_RGBA12;
    case RGBA16:  return GL_RGBA16;
    case SRGB8A8: return GL_SRGB8_ALPHA8;
    case RGBA16F: return GL_RGBA16F;
    case RGBA32F: return GL_RGBA32F;
    }
}
static auto FormatToGL(DepthFormat format) {
    switch (format) {
    using enum DepthFormat;
    case DEPTH16:  return GL_DEPTH_COMPONENT16;
    case DEPTH24:  return GL_DEPTH_COMPONENT24;
    case DEPTH32:  return GL_DEPTH_COMPONENT32;
    case DEPTH32F: return GL_DEPTH_COMPONENT32F;
    }
}
static auto FormatToGL(DepthStencilFormat format) {
    switch (format) {
    using enum DepthStencilFormat;
    case DEPTH24_STENCIL8:  return GL_DEPTH24_STENCIL8;
    case DEPTH32F_STENCIL8: return GL_DEPTH32F_STENCIL8;
    }
}

Texture Texture::CreateTexture(std::string_view name, const char* path, TextureTransparency transparency) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load(path, &width, &height, &nrChannels, transparency == TextureTransparency::YES ? STBI_rgb_alpha : STBI_rgb);
    if (pixelData == nullptr) {
        stbi_image_free(pixelData);
        DOA_LOG_WARNING("Couldn't load %s! No such file at %s!", name.data(), path);
    }

    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, transparency };

    stbi_image_free(pixelData);
    return rv;
}
Texture Texture::CreateTexture(std::string_view name, const unsigned char* data, size_t length, TextureTransparency transparency) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load_from_memory(data, static_cast<int>(length), &width, &height, &nrChannels, transparency == TextureTransparency::YES ? STBI_rgb_alpha : STBI_rgb);
    if (pixelData == nullptr) {
        stbi_image_free(pixelData);
        DOA_LOG_WARNING("Couldn't load %s!", name.data());
    }

    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, transparency };

    stbi_image_free(pixelData);
    return rv;
}
Texture Texture::CreateTexture(std::string_view name, ByteVector data, TextureTransparency transparency){
    return CreateTexture(name, reinterpret_cast<const unsigned char*>(data.data()), data.size(), transparency);
}
Texture Texture::CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, TextureTransparency transparency) {
    if (pixelData == nullptr) {
        DOA_LOG_WARNING("Couldn't load %s from raw pointer to memory! Pointer is nullptr", name.data());
    }

    Texture rv{ name, width, height, pixelData, transparency };

    return rv;
}

const std::byte* const Texture::GetByteBufferOf(const Texture& texture) { return texture._pixelData.data(); }
ByteVector Texture::RequestPixelDataOf(const Texture& texture) { return texture._pixelData; }
void Texture::ApplyPixelDataTo(Texture& texture, const ByteVector& data) {
    Texture::ApplyPixelDataTo(texture, std::move(ByteVector(data)));
}
void Texture::ApplyPixelDataTo(Texture& texture, ByteVector&& data) {
    texture._pixelData = std::move(data);
    texture.DeallocateGPU();
    texture.AllocateGPU();
}

void Texture::Bind(int slot) {
    glBindTextureUnit(slot, _glTextureID);
}

const std::string& Texture::Name() const { return _name; }
size_t Texture::Width() const { return _width; }
size_t Texture::Height() const { return _height; }
bool Texture::HasTransparency() const { return _transparency == TextureTransparency::YES ? true : false; }
TEX Texture::TextureID() const { return _glTextureID; }
void* Texture::TextureIDRaw() const { return reinterpret_cast<void*>(static_cast<uint64_t>(_glTextureID)); }

Texture::~Texture() noexcept {
    DeallocateGPU();
}
Texture::Texture(Texture&& other) noexcept :
    _name(std::move(other._name)),
    _width(std::exchange(other._width, 0)),
    _height(std::exchange(other._height, 0)),
    _transparency(std::exchange(other._transparency, TextureTransparency::NO)),
    _pixelData(std::move(other._pixelData)),
    _glTextureID(std::exchange(other._glTextureID, 0)) {}
Texture& Texture::operator=(Texture&& other) noexcept {
    _name = std::move(other._name);
    _width =  std::exchange(other._width, 0);
    _height = std::exchange(other._height, 0);
    _transparency = std::exchange(other._transparency, TextureTransparency::NO);
    _glTextureID = std::exchange(other._glTextureID, 0);
    _pixelData = std::move(other._pixelData);
    return *this;
}
bool Texture::operator==(const Texture& other) const noexcept {
    if (this == &other) return true;
    /*
        note that we don't need to check _glTextureID
        because if two distinct objects have the same
        _glTextureID, then they are the SAME (which is
        covered by this==&other. why you ask? because if
        they aren't the SAME, same _glTextureID will be
        double-deleted (see dtor). we don't want that.
    */
    if (this->_name == other._name &&
        this->_width == other._width &&
        this->_height == other._height &&
        this->_transparency == other._transparency &&
        this->_pixelData == other._pixelData) {
        return true;
    }
    return false;
}
bool Texture::operator!=(const Texture& other) const noexcept { return !this->operator==(other); }

EncodedTextureData Texture::Serialize(TextureEncoding encoding) const { return SerializeTexture(*this, encoding); }
Texture Texture::Deserialize(const EncodedTextureData& data) { return DeserializeTexture(data); }

Texture Texture::Copy(const Texture& texture) {
    if (texture == Empty()) {
        return {};
    } else {
        return CreateTextureRaw(texture._name, reinterpret_cast<const unsigned char*>(texture._pixelData.data()), texture._width, texture._height, texture._transparency);
    }
}

//-----------------------------------------------------------------

Texture::Texture() noexcept {}
Texture::Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, TextureTransparency transparency) noexcept :
    _name(name),
    _width(width),
    _height(height),
    _transparency(transparency),
    _pixelData(width * height * (transparency == TextureTransparency::YES ? static_cast<size_t>(4) : static_cast<size_t>(3))) {

    for (auto i = 0; i < _pixelData.size(); i++) {
        _pixelData[i] = static_cast<std::byte>(pixelData[i]);
    }
    if (_width > 0 && _height > 0) {
        AllocateGPU();
    } else {
        DOA_LOG_ERROR("Textures must have positive dimensions! Expected width > 0 && height > 0, received width = %d height = %d", _width, _height);
    }
}

void Texture::AllocateGPU() noexcept {
    glCreateTextures(GL_TEXTURE_2D, 1, &_glTextureID);

    glTextureParameteri(_glTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_glTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(_glTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_glTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(
        _glTextureID,
        1,
        _transparency == TextureTransparency::YES ? GL_RGBA8 : GL_RGB8,
        static_cast<GLsizei>(_width),
        static_cast<GLsizei>(_height)
    );
    glTextureSubImage2D(
        _glTextureID,
        0,
        0,
        0,
        static_cast<GLsizei>(_width),
        static_cast<GLsizei>(_height),
        _transparency == TextureTransparency::YES ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        _pixelData.data()
    );
    glGenerateTextureMipmap(_glTextureID);
    glTextureParameterf(_glTextureID, GL_TEXTURE_LOD_BIAS, -0.5f);

}
void Texture::DeallocateGPU() noexcept { glDeleteTextures(1, &_glTextureID); }


Texture2D Texture2D::CreateTextureR(RFormat format, Resolution resolution, ByteVector data) noexcept {
    Texture2D rv;
    rv.Width = resolution.Width;
    rv.Height = resolution.Height;
    rv.Channels = 1;
    rv.PixelData = std::move(data);

    glCreateTextures(GL_TEXTURE_2D, 1, &rv.ID);
    glTextureStorage2D(
        rv.ID,
        static_cast<GLsizei>(1 + std::floor(std::log2(std::max(rv.Width, rv.Height)))),
        FormatToGL(format),
        static_cast<GLsizei>(rv.Width),
        static_cast<GLsizei>(rv.Height)
    );
    if (!data.empty()) {
        glTextureSubImage2D(
            rv.ID,
            0,
            0,
            0,
            static_cast<GLsizei>(rv.Width),
            static_cast<GLsizei>(rv.Height),
            GL_R,
            GL_UNSIGNED_BYTE,
            rv.PixelData.data()
        );
        glGenerateTextureMipmap(rv.ID);
    }

    return rv;
}
Texture2D Texture2D::CreateTextureRG(RGFormat format, Resolution resolution, ByteVector data) noexcept {
    Texture2D rv;
    rv.Width = resolution.Width;
    rv.Height = resolution.Height;
    rv.Channels = 2;
    rv.PixelData = std::move(data);

    glCreateTextures(GL_TEXTURE_2D, 1, &rv.ID);
    glTextureStorage2D(
        rv.ID,
        static_cast<GLsizei>(1 + std::floor(std::log2(std::max(rv.Width, rv.Height)))),
        FormatToGL(format),
        static_cast<GLsizei>(rv.Width),
        static_cast<GLsizei>(rv.Height)
    );
    if (!data.empty()) {
        glTextureSubImage2D(
            rv.ID,
            0,
            0,
            0,
            static_cast<GLsizei>(rv.Width),
            static_cast<GLsizei>(rv.Height),
            GL_RG,
            GL_UNSIGNED_BYTE,
            rv.PixelData.data()
        );
        glGenerateTextureMipmap(rv.ID);
    }

    return rv;
}
Texture2D Texture2D::CreateTextureRGB(RGBFormat format, Resolution resolution, ByteVector data) noexcept {
    Texture2D rv;
    rv.Width = resolution.Width;
    rv.Height = resolution.Height;
    rv.Channels = 3;
    rv.PixelData = std::move(data);

    glCreateTextures(GL_TEXTURE_2D, 1, &rv.ID);
    glTextureStorage2D(
        rv.ID,
        static_cast<GLsizei>(1 + std::floor(std::log2(std::max(rv.Width, rv.Height)))),
        FormatToGL(format),
        static_cast<GLsizei>(rv.Width),
        static_cast<GLsizei>(rv.Height)
    );
    if (!data.empty()) {
        glTextureSubImage2D(
            rv.ID,
            0,
            0,
            0,
            static_cast<GLsizei>(rv.Width),
            static_cast<GLsizei>(rv.Height),
            GL_RGB,
            GL_UNSIGNED_BYTE,
            rv.PixelData.data()
        );
        glGenerateTextureMipmap(rv.ID);
    }

    return rv;
}
Texture2D Texture2D::CreateTextureRGBA(RGBAFormat format, Resolution resolution, ByteVector data) noexcept {
    Texture2D rv;
    rv.Width = resolution.Width;
    rv.Height = resolution.Height;
    rv.Channels = 4;
    rv.PixelData = std::move(data);

    glCreateTextures(GL_TEXTURE_2D, 1, &rv.ID);
    glTextureStorage2D(
        rv.ID,
        static_cast<GLsizei>(1 + std::floor(std::log2(std::max(rv.Width, rv.Height)))),
        FormatToGL(format),
        static_cast<GLsizei>(rv.Width),
        static_cast<GLsizei>(rv.Height)
    );
    if (!data.empty()) {
        glTextureSubImage2D(
            rv.ID,
            0,
            0,
            0,
            static_cast<GLsizei>(rv.Width),
            static_cast<GLsizei>(rv.Height),
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rv.PixelData.data()
        );
        glGenerateTextureMipmap(rv.ID);
    }

    return rv;
}
Texture2D Texture2D::CreateTextureDepth(DepthFormat format, Resolution resolution, ByteVector data) noexcept {
    Texture2D rv;
    rv.Width = resolution.Width;
    rv.Height = resolution.Height;
    rv.Channels = 2;
    rv.PixelData = std::move(data);

    glCreateTextures(GL_TEXTURE_2D, 1, &rv.ID);
    glTextureStorage2D(
        rv.ID,
        static_cast<GLsizei>(1 + std::floor(std::log2(std::max(rv.Width, rv.Height)))),
        FormatToGL(format),
        static_cast<GLsizei>(rv.Width),
        static_cast<GLsizei>(rv.Height)
    );
    if (!data.empty()) {
        glTextureSubImage2D(
            rv.ID,
            0,
            0,
            0,
            static_cast<GLsizei>(rv.Width),
            static_cast<GLsizei>(rv.Height),
            GL_DEPTH_COMPONENT,
            GL_UNSIGNED_BYTE,
            rv.PixelData.data()
        );
        glGenerateTextureMipmap(rv.ID);
    }

    return rv;
}
Texture2D Texture2D::CreateTextureDepthStencil(DepthStencilFormat format, Resolution resolution, ByteVector data) noexcept {
    Texture2D rv;
    rv.Width = resolution.Width;
    rv.Height = resolution.Height;
    rv.Channels = 2;
    rv.PixelData = std::move(data);

    glCreateTextures(GL_TEXTURE_2D, 1, &rv.ID);
    glTextureStorage2D(
        rv.ID,
        static_cast<GLsizei>(1 + std::floor(std::log2(std::max(rv.Width, rv.Height)))),
        FormatToGL(format),
        static_cast<GLsizei>(rv.Width),
        static_cast<GLsizei>(rv.Height)
    );
    if (!data.empty()) {
        glTextureSubImage2D(
            rv.ID,
            0,
            0,
            0,
            static_cast<GLsizei>(rv.Width),
            static_cast<GLsizei>(rv.Height),
            GL_DEPTH_STENCIL,
            GL_UNSIGNED_BYTE,
            rv.PixelData.data()
        );
        glGenerateTextureMipmap(rv.ID);
    }

    return rv;
}
