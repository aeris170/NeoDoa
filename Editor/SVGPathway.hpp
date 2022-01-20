#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <Color.hpp>
#include <Texture.hpp>

enum class TextureStyle {
	NONE,
	PADDED,
	SCALED
};

struct SVGPathway {

	static void Initialize(Color&& color = { 1, 1, 1, 1 });

	static std::weak_ptr<Texture> Get(const std::string& key, const TextureStyle style = TextureStyle::NONE);

private:
	static constexpr const char* const PATH{ "SVGs" };
	static constexpr int WIDTH{ 1024 };
	static constexpr int HEIGHT{ 1024 };
	static constexpr float SCALE_FACTOR{ .25f };
	static constexpr uint32_t RASTER_BG_COLOR{ 0xffffff00 };

	static bool Initialized;
	static std::unordered_map<std::string, std::weak_ptr<Texture>> Textures;
	static std::unordered_map<std::string, std::weak_ptr<Texture>> TexturesPadded;
	static std::unordered_map<std::string, std::weak_ptr<Texture>> TexturesScaled;
};

