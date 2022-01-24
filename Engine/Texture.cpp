#include "Texture.hpp"

#include <unordered_map>

#include <stb_image.h>

#include "Log.hpp"

static std::unordered_map<std::string, std::shared_ptr<Texture>> TEXTURES;

bool Texture::FACTORY_FLAG{ false };

Texture::Texture(std::string_view name, int width, int height, const unsigned char* const pixelData, bool hasTransparency) noexcept :
	_name(name),
	_pixelData(size_t(width)* height * (hasTransparency ? size_t(4) : size_t(3))) {
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

void Texture::Bind(int slot) {
	glBindTextureUnit(slot, _glTextureID);
}

//-----------------------------------------------------------------

std::weak_ptr<Texture> CreateTexture(std::string_view name, const char* path, bool hasTransparency) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* pixelData = stbi_load(path, &width, &height, &nrChannels, hasTransparency ? STBI_rgb_alpha : STBI_rgb);
	if (pixelData == nullptr) {
		stbi_image_free(pixelData);
		DOA_LOG_WARNING("Couldn't load %s! No such file at %s!", name.data(), path);
		return {};
	}

#ifdef _DEBUG
	Texture::FACTORY_FLAG = true;
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
	Texture::FACTORY_FLAG = false;
#else
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
#endif
	TEXTURES.emplace(std::string(name), rv);
	stbi_image_free(pixelData);
	return rv;
}

std::weak_ptr<Texture> CreateTexture(std::string_view name, const unsigned char* data, int length, bool hasTransparency) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* pixelData = stbi_load_from_memory(data, length, &width, &height, &nrChannels, hasTransparency ? STBI_rgb_alpha : STBI_rgb);
	if (pixelData == nullptr) {
		stbi_image_free(pixelData);
		DOA_LOG_WARNING("Couldn't load %s!");
		return {};
	}

#ifdef _DEBUG
	Texture::FACTORY_FLAG = true;
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
	Texture::FACTORY_FLAG = false;
#else
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
#endif
	TEXTURES.emplace(std::string(name), rv);
	stbi_image_free(pixelData);
	return rv;

}

std::weak_ptr<Texture> CreateTextureRaw(std::string_view name, const unsigned char* pixelData, int width, int height, bool hasTransparency) {
	if (pixelData == nullptr) {
		DOA_LOG_WARNING("Couldn't load %s from raw pointer to memory! Pointer is nullptr");
		return {};
	}
#ifdef _DEBUG
	Texture::FACTORY_FLAG = true;
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
	Texture::FACTORY_FLAG = false;
#else
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
#endif
	TEXTURES.emplace(std::string(name), rv);
	return rv;
}

std::weak_ptr<Texture> FindTexture(std::string_view name) {
	auto it = TEXTURES.find(name.data());
	if (it == TEXTURES.end()) {
		DOA_LOG_WARNING("FindTexture failed. There is no Texture named %s!", name);
		return {};
	}
	return it->second;
}
