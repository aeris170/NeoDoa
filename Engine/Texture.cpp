#include "Texture.hpp"

#include <unordered_map>

#include <stb_image.h>

#include "Log.hpp"

static std::unordered_map<std::string, std::shared_ptr<Texture>> TEXTURES;

Texture::Texture(std::string_view name, int width, int height, unsigned char* pixelData, bool hasTransparency) noexcept :
	_name(name),
	_pixelData(pixelData) {
	glGenTextures(1, &_glTextureID);
	Bind(18);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, hasTransparency ? GL_RGBA : GL_RGB, width, height, 0, hasTransparency ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixelData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() noexcept {
	glDeleteTextures(1, &_glTextureID);
	stbi_image_free(_pixelData);
}

Texture::Texture(Texture&& other) noexcept {
	*this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept {
	_name = std::move(other._name);
	_glTextureID = other._glTextureID;
	other._glTextureID = 0;
	_pixelData = other._pixelData;
	other._pixelData = nullptr;
	return *this;
}

void Texture::Bind(int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, _glTextureID);
}

//-----------------------------------------------------------------

std::optional<std::weak_ptr<Texture>> CreateTexture(std::string_view name, const char* path, bool hasTransparency) {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pixelData = stbi_load(path, &width, &height, &nrChannels, hasTransparency ? STBI_rgb_alpha : STBI_rgb);

	if (pixelData == nullptr) {
		stbi_image_free(pixelData);
		DOA_LOG_WARNING("Couldn't load %s! No such file at %s!", name.data(), path);
		return {};
	}
	auto rv = std::make_shared<Texture>(name, width, height, pixelData, hasTransparency);
	TEXTURES.emplace(std::string(name), rv);
	return rv;
}

std::optional<std::weak_ptr<Texture>> FindTexture(std::string_view name) {
	auto it = TEXTURES.find(name.data());
	if (it == TEXTURES.end()) {
		DOA_LOG_WARNING("FindTexture failed. There is no Texture named %s!", name);
		return {};
	}
	return it->second;
}
