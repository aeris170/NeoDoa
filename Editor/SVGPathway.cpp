#include <Editor/SVGPathway.hpp>

#include <format>
#include <string>
#include <vector>
#include <sstream>
#include <numeric>
#include <iostream>
#include <string_view>

#include <tinyxml2.h>
#include <lunasvg.h>

#include <Utility/Split.hpp>

#include <Engine/Log.hpp>

void SVGPathway::Initialize(const std::filesystem::path& directory, Color color) {
    SVGPathway::directory = directory;
    SVGPathway::color = color;
    Initialized = true;
}
void SVGPathway::Initialize(std::filesystem::path&& directory, Color color) {
    SVGPathway::directory = std::move(directory);
    SVGPathway::color = color;
    Initialized = true;
}

const GPUTexture& SVGPathway::Get(const std::string& key, const TextureStyle style, const TextureSize size) {
    assert(Initialized);
    switch (style) {
    using enum TextureStyle;
    case NONE:
        if (!Textures.contains(key)) {
            Load(key);
        }
        return Textures.at(key)[static_cast<size_t>(size)];
    case PADDED:
        if (!TexturesPadded.contains(key)) {
            Load(key);
        }
        return TexturesPadded.at(key)[static_cast<size_t>(size)];
    case SCALED:
        if (!TexturesScaled.contains(key)) {
            Load(key);
        }
        return TexturesScaled.at(key)[static_cast<size_t>(TextureSize::SMALL)]; /* see ref. [1] */
    }
    assert(false); /* no such texture */
    throw 1;
}

void SVGPathway::Load(std::string_view key) {
    assert(Initialized);

    std::string name{ key };
    std::filesystem::path path = directory / PATH / (name + EXT);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(path.string().c_str());
    if (err != tinyxml2::XML_SUCCESS) {
        DOA_LOG_WARNING("Could not read SVG: %s", key.data());
        return;
    } else {
        DOA_LOG_TRACE("Reading SVG: %s", key.data());
    }

    tinyxml2::XMLElement* rootNode = doc.RootElement();
    for (tinyxml2::XMLElement* node = rootNode->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()) {
        std::string colorString("#");
        colorString += std::format("{:02x}", int(color.r * 255.999));
        colorString += std::format("{:02x}", int(color.g * 255.999));
        colorString += std::format("{:02x}", int(color.b * 255.999));

        node->SetAttribute("fill", colorString.c_str());
    }

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    std::unique_ptr<lunasvg::Document> lunadoc = lunasvg::Document::loadFromData(printer.CStr());

    const char* viewBox{ nullptr };
    rootNode->QueryAttribute("viewBox", &viewBox);
    std::vector<std::string> tokens = split(viewBox, " ");
    std::vector<float> box{
        static_cast<float>(stoi(tokens[0])),
        static_cast<float>(stoi(tokens[1])),
        static_cast<float>(stoi(tokens[2])),
        static_cast<float>(stoi(tokens[3]))
    };

    auto convertToSpan = [](uint8_t* ptr, size_t size) -> std::span<std::byte> {
        return std::span<std::byte>(reinterpret_cast<std::byte*>(ptr), size);
    };

    { // Fill the "Textures"
        // Small
        lunasvg::Bitmap bitmapScaledAspect;
        bitmapScaledAspect = lunadoc->renderToBitmap(
            static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_SMALL),
            static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_SMALL),
            RASTER_BG_COLOR
        );
        GPUTextureBuilder builder;
        builder.SetName("!!" + name + "_none_small!!")
            .SetWidth(bitmapScaledAspect.width())
            .SetHeight(bitmapScaledAspect.height())
            .SetData(DataFormat::RGBA8, convertToSpan(bitmapScaledAspect.data(), bitmapScaledAspect.width() * bitmapScaledAspect.height() * 4));
        auto [texSmall, _] = builder.Build();

        // Medium
        bitmapScaledAspect = lunadoc->renderToBitmap(
            static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_MEDIUM),
            static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_MEDIUM),
            RASTER_BG_COLOR
        );
        builder.SetName("!!" + name + "_none_medium!!")
            .SetWidth(bitmapScaledAspect.width())
            .SetHeight(bitmapScaledAspect.height())
            .SetData(DataFormat::RGBA8, convertToSpan(bitmapScaledAspect.data(), bitmapScaledAspect.width() * bitmapScaledAspect.height() * 4));
        auto [texMedium, __] = builder.Build();

        // Large
        bitmapScaledAspect = lunadoc->renderToBitmap(
            static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_LARGE),
            static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_LARGE),
            RASTER_BG_COLOR
        );
        builder.SetName("!!" + name + "_none_large!!")
            .SetWidth(bitmapScaledAspect.width())
            .SetHeight(bitmapScaledAspect.height())
            .SetData(DataFormat::RGBA8, convertToSpan(bitmapScaledAspect.data(), bitmapScaledAspect.width() * bitmapScaledAspect.height() * 4));
        auto [texLarge, ___] = builder.Build();

        if (texSmall && texMedium && texLarge) {
            Textures.try_emplace(name, std::move(texSmall.value()), std::move(texMedium.value()), std::move(texLarge.value()));
        }
    }

    { // Fill the "TexturesPadded"
        auto paddingFunction = [](std::unique_ptr<lunasvg::Document>& lunadoc, const std::vector<float>& box, const std::string& name, std::string_view scaleFactorName, float scaleFactor) {
            auto convertToSpan = [](uint32_t* ptr, size_t size) -> std::span<std::byte> {
                return std::span<std::byte>(reinterpret_cast<std::byte*>(ptr), size);
            };

            lunasvg::Bitmap bitmapPadded;
            bitmapPadded = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * scaleFactor),
                static_cast<uint32_t>((box[3] - box[1]) * scaleFactor),
                RASTER_BG_COLOR
            );
            const size_t width = bitmapPadded.width();
            const size_t height = bitmapPadded.height();
            const uint8_t elemWidth = 4U;
            const size_t dimension = size_t(std::max(width, height));
            uint32_t* paddedBuf = new uint32_t[dimension * dimension](); // zero initialized rgba buffer
            const auto* buffer = bitmapPadded.data();
            size_t bufIdx = 0;

            bool widthScaled = width != dimension;
            for (size_t y = 0; y < dimension; y++) {
                for (size_t x = 0; x < dimension; x++) {
                    unsigned char* currentElement{ nullptr };
                    if (widthScaled) {
                        // pad left-right
                        if (x >= (dimension - width) / 2 && x < std::midpoint(dimension, width)) {
                            currentElement = (unsigned char*) &paddedBuf[x + y * dimension];
                        }
                    } else {
                        // pad top-bottom
                        if (y >= (dimension - height) / 2 && y < std::midpoint(dimension, height)) {
                            currentElement = (unsigned char*) &paddedBuf[x + y * dimension];
                        }
                    }
                    if (currentElement == nullptr) continue;

                    currentElement[0] = buffer[bufIdx];
                    currentElement[1] = buffer[bufIdx + 1];
                    currentElement[2] = buffer[bufIdx + 2];
                    currentElement[3] = buffer[bufIdx + 3];
                    bufIdx += elemWidth;
                    if (bufIdx > width * height * elemWidth) {
                        bufIdx = 0;
                    }
                }
            }

            GPUTextureBuilder builder;
            builder.SetName("!!" + name + "_padded_" + scaleFactorName.data() + "!!")
                .SetWidth(static_cast<unsigned>(dimension))
                .SetHeight(static_cast<unsigned>(dimension))
                .SetData(DataFormat::RGBA8, convertToSpan(paddedBuf, width * height * sizeof(uint32_t)));
            auto [tex, _] = builder.Build();

            delete[] paddedBuf;
            return std::move(tex);
        };

        auto smallTex = paddingFunction(lunadoc, box, name, "small", SCALE_FACTOR_SMALL);
        auto mediumTex = paddingFunction(lunadoc, box, name, "medium", SCALE_FACTOR_MEDIUM);
        auto largeTex = paddingFunction(lunadoc, box, name, "large", SCALE_FACTOR_LARGE);

        if (smallTex && mediumTex && largeTex) {
            TexturesPadded.try_emplace(name, std::move(smallTex.value()), std::move(mediumTex.value()), std::move(largeTex.value()));
        }
    }

    lunasvg::Bitmap bitmapScaled;
    { // Fill the "TexturesScaled"
        bitmapScaled = lunadoc->renderToBitmap(WIDTH, HEIGHT, RASTER_BG_COLOR);

        GPUTextureBuilder builder;
        builder.SetName("!!" + name + "_scaled!!")
            .SetWidth(bitmapScaled.width())
            .SetHeight(bitmapScaled.height())
            .SetData(DataFormat::RGBA8, convertToSpan(bitmapScaled.data(), bitmapScaled.width() * bitmapScaled.height() * 4));
        auto [tex, _] = builder.Build();

        if (tex) {
            /* we cannot copy (we can but why copy?), there is no "size" defined for scaled texture. if there were, it
            wouldn't make sense. "Hey SVG, pls give me A SMALL SCALED texture", what even is a SMALL SCALED texture? there is only one
            scaled texture and it is placed to the beginning of the pack. for future: ref. [1] */
            TexturesScaled.try_emplace(name, std::move(tex.value()), GPUTexture{}, GPUTexture{});
        }
    }
}

GPUTexture& SVGPathway::TexturePack::operator[](size_t index) noexcept {
    assert(index >= 0 && index < 3);
    switch(index){
        case 0: return SmallTexture;
        case 1: return MediumTexture;
        case 2: return LargeTexture;
    }
    std::abort();
}
