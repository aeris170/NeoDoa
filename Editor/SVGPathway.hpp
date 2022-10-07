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

enum class TextureSize {
    SMALL,
    MEDIUM,
    LARGE,
    SIZE_COUNT /* = 3 */
};

struct SVGPathway {

    static void Initialize(Color&& color = { 1, 1, 1, 1 });

    static std::weak_ptr<Texture> Get(const std::string& key, const TextureStyle style = TextureStyle::NONE, const TextureSize size = TextureSize::MEDIUM);

private:
    using TexturePack = std::array<std::weak_ptr<Texture>, static_cast<size_t>(TextureSize::SIZE_COUNT)>;

    static constexpr const char* const PATH{ "SVGs" };
    static constexpr int WIDTH{ 1024 };
    static constexpr int HEIGHT{ 1024 };
    static constexpr float SCALE_FACTOR_SMALL{ .05f };
    static constexpr float SCALE_FACTOR_MEDIUM{ .25f };
    static constexpr float SCALE_FACTOR_LARGE{ .5f };
    static constexpr uint32_t RASTER_BG_COLOR{ 0xffffff00 };

    static bool Initialized;
    static std::unordered_map<std::string, TexturePack> Textures;
    static std::unordered_map<std::string, TexturePack> TexturesPadded;
    static std::unordered_map<std::string, TexturePack> TexturesScaled;
};

