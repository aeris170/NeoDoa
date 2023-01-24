#include "SVGPathway.hpp"

#include <format>
#include <filesystem>
#include <string>
#include <vector>
#include <string_view>
#include <sstream>
#include <iostream>
#include <numeric>

#include <tinyxml2.h>
#include <lunasvg.h>

#include <Log.hpp>

#include <split.hpp>

bool SVGPathway::Initialized{ false };
unordered_string_map<SVGPathway::TexturePack> SVGPathway::Textures{};
unordered_string_map<SVGPathway::TexturePack> SVGPathway::TexturesPadded{};
unordered_string_map<SVGPathway::TexturePack> SVGPathway::TexturesScaled{};

void SVGPathway::Initialize(Color color) {
    assert(!Initialized); /* No double init for SVGPathway. */
    for (auto& p : std::filesystem::recursive_directory_iterator(PATH, std::filesystem::directory_options::skip_permission_denied)) {
        if (!p.path().has_filename()) continue;

        std::filesystem::path path_noext = p.path();
        while (path_noext.has_extension()) {
            path_noext.replace_extension();
        }

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError err = doc.LoadFile(p.path().string().c_str());
        if (err != tinyxml2::XML_SUCCESS) {
            DOA_LOG_WARNING("Could not read SVG: %s", p.path().string());
            continue;
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

        std::string name = path_noext.filename().string().c_str();

        const char* viewBox{ nullptr };
        rootNode->QueryAttribute("viewBox", &viewBox);
        std::vector<std::string> tokens = split(viewBox, " ");
        std::vector<float> box{
            static_cast<float>(stoi(tokens[0])),
            static_cast<float>(stoi(tokens[1])),
            static_cast<float>(stoi(tokens[2])),
            static_cast<float>(stoi(tokens[3]))
        };

        { // Fill the "Textures"
            lunasvg::Bitmap bitmapScaledAspect;
            bitmapScaledAspect = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_SMALL),
                static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_SMALL),
                RASTER_BG_COLOR
            );
            auto small = Texture::CreateTextureRaw("!!" + name + "_none_small!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height());

            bitmapScaledAspect = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_MEDIUM),
                static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_MEDIUM),
                RASTER_BG_COLOR
            );
            auto medium = Texture::CreateTextureRaw("!!" + name + "_none_medium!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height());

            bitmapScaledAspect = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_LARGE),
                static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_LARGE),
                RASTER_BG_COLOR
            );
            auto large = Texture::CreateTextureRaw("!!" + name + "_none_large!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height());

            if (small != Texture::Empty() && medium != Texture::Empty() && large != Texture::Empty()) {
                Textures.try_emplace(name, std::move(small), std::move(medium), std::move(large));
            }
        }

        { // Fill the "TexturesPadded"
            auto paddingFunction = [](std::unique_ptr<lunasvg::Document>& lunadoc, const std::vector<float>& box, const std::string& name, std::string_view scaleFactorName, float scaleFactor) {
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
                auto bufIdx = 0;

                bool widthScaled = width != dimension;
                for (int y = 0; y < dimension; y++) {
                    for (int x = 0; x < dimension; x++) {
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

                auto tex = Texture::CreateTextureRaw("!!" + name + "_padded_" + scaleFactorName.data() + "!!", (unsigned char*) (paddedBuf), dimension, dimension);
                delete[] paddedBuf;
                return tex;
            };

            auto small = paddingFunction(lunadoc, box, name, "small", SCALE_FACTOR_SMALL);
            auto medium = paddingFunction(lunadoc, box, name, "medium", SCALE_FACTOR_MEDIUM);
            auto large = paddingFunction(lunadoc, box, name, "large", SCALE_FACTOR_LARGE);


            if (small != Texture::Empty() && medium != Texture::Empty() && large != Texture::Empty()) {
                TexturesPadded.try_emplace(name, std::move(small), std::move(medium), std::move(large));
            }
        }

        lunasvg::Bitmap bitmapScaled;
        { // Fill the "TexturesScaled"
            bitmapScaled = lunadoc->renderToBitmap(WIDTH, HEIGHT, RASTER_BG_COLOR);
            auto tex = Texture::CreateTextureRaw("!!" + name + "_scaled!!", bitmapScaled.data(), bitmapScaled.width(), bitmapScaled.height());
            if (tex != Texture::Empty()) {
                /* we cannot copy (we can but why copy?), there is no "size" defined for scaled texture. if there were, it
                wouldn't make sense. "Hey SVG, pls give me A SMALL SCALED texture", what even is a SMALL SCALED texture? there is only one
                scaled texture and it is placed to the beginning of the pack. for future: ref. [1] */
                TexturesScaled.try_emplace(name, std::move(tex), Texture::Empty(), Texture::Empty());
            }
        }
    }

    Initialized = true;
}

const Texture& SVGPathway::Get(const std::string& key, const TextureStyle style, const TextureSize size) {
    assert(Initialized); /* SVGPathway not initialized. */
    switch (style) {
    using enum TextureStyle;
    case NONE:
        return Textures.at(key)[static_cast<size_t>(size)];
    case PADDED:
        return TexturesPadded.at(key)[static_cast<size_t>(size)];
    case SCALED:
        return TexturesScaled.at(key)[static_cast<size_t>(TextureSize::SMALL)]; /* see ref. [1] */
    }
    assert(false); /* no such texture */
    throw 1;
}

