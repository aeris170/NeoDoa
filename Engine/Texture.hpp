#pragma once

#include <optional>

#include "TypedefsAndConstants.hpp"

struct Texture : std::enable_shared_from_this<Texture> {
	std::string _name;
	TEX _glTextureID{ 0 };
	unsigned char* _pixelData;

	void Bind(int slot = 0);

	// don't call use CreateTexture
	Texture(std::string_view name, int width, int height, unsigned char* pixelData, bool hasTransparency) noexcept;
	~Texture() noexcept;
	Texture(const Texture&) = delete;
	Texture(Texture&&) noexcept;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) noexcept;

};

std::optional<std::weak_ptr<Texture>> CreateTexture(std::string_view name, const char* path, bool hasTransparency = true);
std::optional<std::weak_ptr<Texture>> FindTexture(std::string_view name);
