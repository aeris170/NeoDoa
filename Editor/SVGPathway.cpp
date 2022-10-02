#include "SVGPathway.hpp"

#include <format>
#include <filesystem>
#include <string>
#include <vector>
#include <string_view>
#include <sstream>
#include <iostream>

#include <tinyxml2.h>
#include <lunasvg.h>

#include <split.hpp>

bool SVGPathway::Initialized{ false };
std::unordered_map<std::string, SVGPathway::TexturePack> SVGPathway::Textures{};
std::unordered_map<std::string, SVGPathway::TexturePack> SVGPathway::TexturesPadded{};
std::unordered_map<std::string, SVGPathway::TexturePack> SVGPathway::TexturesScaled{};

void SVGPathway::Initialize(Color&& color) {
    assert(!Initialized, "No double init for SVGPathway.");
    // string path = "/path/to/directory";
    for (auto& p : std::filesystem::recursive_directory_iterator(PATH, std::filesystem::directory_options::skip_permission_denied)) {
        if (!p.path().has_filename()) continue;

        std::filesystem::path path_noext = p.path();
        while (path_noext.has_extension()) {
            path_noext.replace_extension();
        }

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError err = doc.LoadFile(p.path().string().c_str());

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

        const char* viewBox;
        rootNode->QueryAttribute("viewBox", &viewBox);
        std::vector<std::string> tokens = std::move(split(viewBox, " "));
        std::vector<int> box{ stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]) };

        { // Fill the "Textures"
            lunasvg::Bitmap bitmapScaledAspect;
            bitmapScaledAspect = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_SMALL),
                static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_SMALL),
                RASTER_BG_COLOR
            );
            auto small = CreateTextureRaw("!!" + name + "_none_small!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height(), true);

            bitmapScaledAspect = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_MEDIUM),
                static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_MEDIUM),
                RASTER_BG_COLOR
            );
            auto medium = CreateTextureRaw("!!" + name + "_none_medium!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height(), true);

            bitmapScaledAspect = lunadoc->renderToBitmap(
                static_cast<uint32_t>((box[2] - box[0]) * SCALE_FACTOR_LARGE),
                static_cast<uint32_t>((box[3] - box[1]) * SCALE_FACTOR_LARGE),
                RASTER_BG_COLOR
            );
            auto large = CreateTextureRaw("!!" + name + "_none_large!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height(), true);

            if (!small.expired() && !medium.expired() && !large.expired()) {
                Textures.emplace(name, TexturePack{ small, medium, large });
            }
        }

        { // Fill the "TexturesPadded"
            auto paddingFunction = [](std::unique_ptr<lunasvg::Document>& lunadoc, const std::vector<int>& box, const std::string& name, std::string&& scaleFactorName, float scaleFactor) {
                lunasvg::Bitmap bitmapPadded;
                bitmapPadded = lunadoc->renderToBitmap((box[2] - box[0]) * scaleFactor, (box[3] - box[1]) * scaleFactor, RASTER_BG_COLOR);
                const int width = bitmapPadded.width();
                const int height = bitmapPadded.height();
                const int elemWidth = 4;
                const size_t dimension = size_t(std::max(width, height));
                uint32_t* paddedBuf = new uint32_t[dimension * dimension](); // zero initialized rgba buffer
                unsigned char* buffer = bitmapPadded.data();
                auto bufIdx = 0;

                bool widthScaled = width != dimension;
                for (int y = 0; y < dimension; y++) {
                    for (int x = 0; x < dimension; x++) {
                        unsigned char* currentElement{ nullptr };
                        if (widthScaled) {
                            // pad left-right
                            if (x >= (dimension - width) / 2 && x < (dimension + width) / 2) {
                                currentElement = (unsigned char*)&paddedBuf[x + y * dimension];
                            }
                        }
                        else {
                            // pad top-bottom
                            if (y >= (dimension - height) / 2 && y < (dimension + height) / 2) {
                                currentElement = (unsigned char*)&paddedBuf[x + y * dimension];
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

                auto tex = CreateTextureRaw("!!" + name + "_padded_" + scaleFactorName + "!!", (unsigned char*)(paddedBuf), dimension, dimension, true);
                delete[] paddedBuf;
                return tex;
            };

            auto small = paddingFunction(lunadoc, box, name, "small", SCALE_FACTOR_SMALL);
            auto medium = paddingFunction(lunadoc, box, name, "medium", SCALE_FACTOR_MEDIUM);
            auto large = paddingFunction(lunadoc, box, name, "large", SCALE_FACTOR_LARGE);

            if (!small.expired() && !medium.expired() && !large.expired()) {
                TexturesPadded.emplace(name, TexturePack{ small, medium, large });
            }
        }

        lunasvg::Bitmap bitmapScaled;
        { // Fill the "TexturesScaled"
            bitmapScaled = lunadoc->renderToBitmap(WIDTH, HEIGHT, RASTER_BG_COLOR);
            auto tex = CreateTextureRaw("!!" + name + "_scaled!!", bitmapScaled.data(), bitmapScaled.width(), bitmapScaled.height(), true);
            if (!tex.expired()) {
                TexturesScaled.emplace(name, tex);
            }
        }
    }

    Initialized = true;
}

std::weak_ptr<Texture> SVGPathway::Get(const std::string& key, const TextureStyle style, const TextureSize size) {
    assert(Initialized, "SVGPathway not initialized.");
    switch (style) {
    case TextureStyle::NONE:
        return Textures.at(key)[static_cast<size_t>(size)];
    case TextureStyle::PADDED:
        return TexturesPadded.at(key)[static_cast<size_t>(size)];
    case TextureStyle::SCALED:
        return TexturesScaled.at(key)[static_cast<size_t>(size)];
    }
    assert(false, "no such texture");
}

