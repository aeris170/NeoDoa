#pragma once

#include <string>
#include <vector>

#include <Engine/Mesh.hpp>
#include <Engine/Model.hpp>
#include <Engine/Texture.hpp>
#include <Engine/Material.hpp>
#include <Engine/TextureDeserializer.hpp>

struct FNode;

struct ModelDeserializationResult {
    bool erred{ false };
    std::vector<std::string> errors{};
    Model deserializedModel{};
    std::vector<Mesh> deserializedMeshes{};
    std::vector<size_t> meshMaterialIndices{};
    std::vector<TextureDeserializationResult> deserializedTextures{};
};

ModelDeserializationResult DeserializeModel(const FNode& file) noexcept;
ModelDeserializationResult DeserializeModel(const std::string_view data) noexcept;