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
std::unordered_map<std::string, std::weak_ptr<Texture>> SVGPathway::Textures{};
std::unordered_map<std::string, std::weak_ptr<Texture>> SVGPathway::TexturesPadded{};
std::unordered_map<std::string, std::weak_ptr<Texture>> SVGPathway::TexturesScaled{};

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

        lunasvg::Bitmap bitmapScaledAspect;
        const char* viewBox;
        rootNode->QueryAttribute("viewBox", &viewBox);
        std::vector<std::string> tokens = std::move(split(viewBox, " "));
        std::vector<int> box{ stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]) };

        bitmapScaledAspect = lunadoc->renderToBitmap((box[2] - box[0]) * SCALE_FACTOR, (box[3] - box[1]) * SCALE_FACTOR, RASTER_BG_COLOR);

        auto tex = CreateTextureRaw("!!" + name + "_none!!", bitmapScaledAspect.data(), bitmapScaledAspect.width(), bitmapScaledAspect.height(), true);
        if (!tex.expired()) {
            Textures.emplace(name, tex);
        }

        lunasvg::Bitmap bitmapPadded;
        bitmapPadded = lunadoc->renderToBitmap((box[2] - box[0]) * SCALE_FACTOR, (box[3] - box[1]) * SCALE_FACTOR, RASTER_BG_COLOR);
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
                } else {
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

        tex = CreateTextureRaw("!!" + name + "_padded!!", (unsigned char*)(paddedBuf), dimension, dimension, true);
        if (!tex.expired()) {
            TexturesPadded.emplace(name, tex);
        }
        delete[] paddedBuf;

        lunasvg::Bitmap bitmapScaled;
        bitmapScaled = lunadoc->renderToBitmap(WIDTH, HEIGHT, RASTER_BG_COLOR);
        tex = CreateTextureRaw("!!" + name + "_scaled!!", bitmapScaled.data(), bitmapScaled.width(), bitmapScaled.height(), true);
        if (!tex.expired()) {
            TexturesScaled.emplace(name, tex);
        }
    }

    Initialized = true;
}

std::weak_ptr<Texture> SVGPathway::Get(const std::string& key, const TextureStyle style) {
    assert(Initialized, "SVGPathway not initialized.");
    switch (style) {
    case TextureStyle::NONE:
        return Textures.at(key);
    case TextureStyle::PADDED:
        return TexturesPadded.at(key);
    case TextureStyle::SCALED:
        return TexturesScaled.at(key);
    }
    assert(false, "no such texture");
}

