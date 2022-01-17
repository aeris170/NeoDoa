#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <Color.hpp>
#include <Texture.hpp>

struct SVGPathway {

	static void Initialize(Color&& color = { 1, 1, 1, 1 });

	static std::weak_ptr<Texture> Get(const std::string& key);

private:
	static constexpr const char* const PATH{ "SVGs" };
	static constexpr int WIDTH{ 0 };
	static constexpr int HEIGHT{ 0 };
	static constexpr uint32_t RASTER_BG_COLOR{ 0xffffff00 };

	static bool Initialized;
	static std::unordered_map<std::string, std::weak_ptr<Texture>> Textures;
};

