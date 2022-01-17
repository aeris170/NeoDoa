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

        lunasvg::Bitmap bitmap;
        if constexpr (WIDTH == 0 || HEIGHT == 0) {
            const char* viewBox;
            rootNode->QueryAttribute("viewBox", &viewBox);
            std::vector<std::string> tokens = std::move(split(viewBox, " "));
            std::vector<int> box{ stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]) };

            bitmap = lunadoc->renderToBitmap(box[2] - box[0], box[3] - box[1], RASTER_BG_COLOR);
        } else {
            bitmap = lunadoc->renderToBitmap(WIDTH, HEIGHT, RASTER_BG_COLOR);
        }

        std::string name = path_noext.filename().string().c_str();
        auto tex = CreateTextureRaw(name, bitmap.data(), bitmap.width(), bitmap.height(), true);
        if(!tex.expired()) {
            Textures.emplace(name, tex);
        }
    }

    Initialized = true;
}

std::weak_ptr<Texture> SVGPathway::Get(const std::string& key) {
    assert(Initialized, "SVGPathway not initialized.");
    return Textures.at(key);
}

