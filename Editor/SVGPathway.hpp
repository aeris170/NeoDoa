#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <string_map.hpp>

#include <Color.hpp>
#include <Texture.hpp>

enum class TextureStyle {
    NONE,
    PADDED,
    SCALED
};

enum class TextureSize {
    SMALL,
    MEDIUM,
    LARGE,
    SIZE_COUNT /* = 3 */
};

struct SVGPathway {

    static void Initialize(Color color = { 1, 1, 1, 1 });

    static const Texture& Get(const std::string& key, const TextureStyle style = TextureStyle::NONE, const TextureSize size = TextureSize::MEDIUM);

private:
    using TexturePack = std::array<Texture, static_cast<size_t>(TextureSize::SIZE_COUNT)>;

    static constexpr const char* const PATH{ "SVGs" };
    static constexpr int WIDTH{ 1024 };
    static constexpr int HEIGHT{ 1024 };
    static constexpr float SCALE_FACTOR_SMALL{ .05f };
    static constexpr float SCALE_FACTOR_MEDIUM{ .25f };
    static constexpr float SCALE_FACTOR_LARGE{ .5f };
    static constexpr uint32_t RASTER_BG_COLOR{ 0xffffff00 };

    static bool Initialized;
    static unordered_string_map<SVGPathway::TexturePack> Textures;
    static unordered_string_map<SVGPathway::TexturePack> TexturesPadded;
    static unordered_string_map<SVGPathway::TexturePack> TexturesScaled;
};

