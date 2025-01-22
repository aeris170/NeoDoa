#pragma once

#include <string>
#include <vector>

#include <Engine/Model.hpp>
#include <Engine/Mesh.hpp>
#include <Engine/Material.hpp>
#include <Engine/Texture.hpp>

struct ModelDeserializer {
    bool erred{ false };
    std::vector<std::string> errors{};
    Model deserializedModel{};
    std::vector<Mesh> deserializedMeshes{};
    std::vector<Material> deserializedMaterials{};
    std::vector<Texture> deserializedTextures{};
};