#include "Texture.hpp"

#include <unordered_map>

#include <stb_image.h>

#include "Log.hpp"

Texture Texture::CreateTexture(std::string_view name, const char* path, bool hasTransparency) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load(path, &width, &height, &nrChannels, hasTransparency ? STBI_rgb_alpha : STBI_rgb);
    if (pixelData == nullptr) {
        stbi_image_free(pixelData);
        DOA_LOG_WARNING("Couldn't load %s! No such file at %s!", name.data(), path);
        return Empty();
    }

#ifdef _DEBUG
    Texture::FACTORY_FLAG = true;
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, hasTransparency };
    Texture::FACTORY_FLAG = false;
#else
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, hasTransparency };
#endif
    stbi_image_free(pixelData);
    return rv;
}
Texture Texture::CreateTexture(std::string_view name, const unsigned char* data, size_t length, bool hasTransparency) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load_from_memory(data, length, &width, &height, &nrChannels, hasTransparency ? STBI_rgb_alpha : STBI_rgb);
    if (pixelData == nullptr) {
        stbi_image_free(pixelData);
        DOA_LOG_WARNING("Couldn't load %s!");
        return Empty();
    }

#ifdef _DEBUG
    Texture::FACTORY_FLAG = true;
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, hasTransparency };
    Texture::FACTORY_FLAG = false;
#else
    TexturePtr rv = TexturePtr(new Texture(name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, hasTransparency));
#endif
    stbi_image_free(pixelData);
    return rv;
}
Texture Texture::CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, bool hasTransparency) {
    if (pixelData == nullptr) {
        DOA_LOG_WARNING("Couldn't load %s from raw pointer to memory! Pointer is nullptr");
        return Empty();
    }
#ifdef _DEBUG
    Texture::FACTORY_FLAG = true;
    Texture rv{ name, width, height, pixelData, hasTransparency };
    Texture::FACTORY_FLAG = false;
#else
    Texture rv{ name, width, height, pixelData, hasTransparency };
#endif
    return rv;
}

void Texture::Bind(int slot) {
    glBindTextureUnit(slot, _glTextureID);
}

const std::string& Texture::Name() const { return _name; }
TEX Texture::TextureID() const { return _glTextureID; }

Texture::~Texture() noexcept {
    glDeleteTextures(1, &_glTextureID);
}
Texture::Texture(Texture&& other) noexcept :
    _name(std::move(other._name)),
    _pixelData(std::move(other._pixelData)),
    _glTextureID(std::exchange(other._glTextureID, 0)) {}
Texture& Texture::operator=(Texture&& other) noexcept {
    _name = std::move(other._name);
    _glTextureID = std::exchange(other._glTextureID, 0);
    _pixelData = std::move(other._pixelData);
    return *this;
}
bool Texture::operator==(const Texture& other) {
    if (this == &other) return true;
    /*
        note that we don't need to check _glTextureID
        because if two distinct objects have the same
        _glTextureID, then they are the SAME (which is
        covered by this==&other. why you ask? because if
        they aren't the SAME, same _glTextureID will be
        double-deleted (see dtor). we don't want that.
    */
    if (this->_name == other._name && this->_pixelData == other._pixelData) return true;
    return false;
}
bool Texture::operator!=(const Texture& other) { return !this->operator==(other); }

//-----------------------------------------------------------------

Texture::Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, bool hasTransparency) noexcept :
    _name(name),
    _pixelData(width* height* (hasTransparency ? static_cast<size_t>(4) : static_cast<size_t>(3))) {
    assert(FACTORY_FLAG, "don't call ctor directly, use CreateTexture");
    for (auto i = 0; i < _pixelData.size(); i++) {
        _pixelData[i] = pixelData[i];
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &_glTextureID);

    glTextureParameteri(_glTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_glTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(_glTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_glTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(_glTextureID, 1, hasTransparency ? GL_RGBA8 : GL_RGB8, width, height);
    glTextureSubImage2D(_glTextureID, 0, 0, 0, width, height, hasTransparency ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _pixelData.data());
    glGenerateTextureMipmap(_glTextureID);
    glTextureParameterf(_glTextureID, GL_TEXTURE_LOD_BIAS, -0.5f);
}