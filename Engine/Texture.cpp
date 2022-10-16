#include "Texture.hpp"

#include <unordered_map>

#include <stb_image.h>

#include "Log.hpp"

Texture Texture::CreateTexture(std::string_view name, const char* path, Transparency transparency) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load(path, &width, &height, &nrChannels, transparency == Transparency::YES ? STBI_rgb_alpha : STBI_rgb);
    if (pixelData == nullptr) {
        stbi_image_free(pixelData);
        DOA_LOG_WARNING("Couldn't load %s! No such file at %s!", name.data(), path);
        return Empty();
    }

#ifdef _DEBUG
    Texture::FACTORY_FLAG = true;
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, transparency };
    Texture::FACTORY_FLAG = false;
#else
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, transparency };
#endif
    stbi_image_free(pixelData);
    return rv;
}
Texture Texture::CreateTexture(std::string_view name, const unsigned char* data, size_t length, Transparency transparency) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load_from_memory(data, length, &width, &height, &nrChannels, transparency == Transparency::YES ? STBI_rgb_alpha : STBI_rgb);
    if (pixelData == nullptr) {
        stbi_image_free(pixelData);
        DOA_LOG_WARNING("Couldn't load %s!");
        return Empty();
    }

#ifdef _DEBUG
    Texture::FACTORY_FLAG = true;
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, transparency };
    Texture::FACTORY_FLAG = false;
#else
    Texture rv{ name, static_cast<size_t>(width), static_cast<size_t>(height), pixelData, transparency };
#endif
    stbi_image_free(pixelData);
    return rv;
}
Texture Texture::CreateTextureRaw(std::string_view name, const unsigned char* pixelData, size_t width, size_t height, Transparency transparency) {
    if (pixelData == nullptr) {
        DOA_LOG_WARNING("Couldn't load %s from raw pointer to memory! Pointer is nullptr");
        return Empty();
    }
#ifdef _DEBUG
    Texture::FACTORY_FLAG = true;
    Texture rv{ name, width, height, pixelData, transparency };
    Texture::FACTORY_FLAG = false;
#else
    Texture rv{ name, width, height, pixelData, transparency };
#endif
    return rv;
}

Texture::ByteVector Texture::RequestPixelDataOf(const Texture& texture) { return texture._pixelData; }
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
bool Texture::HasTransparency() const { return _transparency == Transparency::YES ? true : false; }
TEX Texture::TextureID() const { return _glTextureID; }

Texture::~Texture() noexcept {
    DeallocateGPU();
}
Texture::Texture(Texture&& other) noexcept :
    _name(std::move(other._name)),
    _width(std::exchange(other._width, 0)),
    _height(std::exchange(other._height, 0)),
    _transparency(std::exchange(other._transparency, Transparency::NO)),
    _pixelData(std::move(other._pixelData)),
    _glTextureID(std::exchange(other._glTextureID, 0)) {}
Texture& Texture::operator=(Texture&& other) noexcept {
    _name = std::move(other._name);
    _width =  std::exchange(other._width, 0);
    _height = std::exchange(other._height, 0);
    _transparency = std::exchange(other._transparency, Transparency::NO);
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
    if (this->_name == other._name &&
        this->_width == other._width &&
        this->_height == other._height &&
        this->_transparency == other._transparency &&
        this->_pixelData == other._pixelData) {
        return true;
    }
    return false;
}
bool Texture::operator!=(const Texture& other) { return !this->operator==(other); }

Texture Texture::Copy(const Texture& scene) { return Empty(); }

//-----------------------------------------------------------------

Texture::Texture(std::string_view name, size_t width, size_t height, const unsigned char* const pixelData, Transparency transparency) noexcept :
    _name(name),
    _width(width),
    _height(height),
    _transparency(transparency),
    _pixelData(width * height * (transparency == Transparency::YES ? static_cast<size_t>(4) : static_cast<size_t>(3))) {
    assert(FACTORY_FLAG, "don't call ctor directly, use CreateTexture");
    for (auto i = 0; i < _pixelData.size(); i++) {
        _pixelData[i] = pixelData[i];
    }
    AllocateGPU();
}

void Texture::AllocateGPU() {
    glCreateTextures(GL_TEXTURE_2D, 1, &_glTextureID);

    glTextureParameteri(_glTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_glTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(_glTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_glTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(_glTextureID, 1, _transparency == Transparency::YES ? GL_RGBA8 : GL_RGB8, _width, _height);
    glTextureSubImage2D(_glTextureID, 0, 0, 0, _width, _height, _transparency == Transparency::YES ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _pixelData.data());
    glGenerateTextureMipmap(_glTextureID);
    glTextureParameterf(_glTextureID, GL_TEXTURE_LOD_BIAS, -0.5f);

}
void Texture::DeallocateGPU() { glDeleteTextures(1, &_glTextureID); }