#pragma once

#include <string>
#include <vector>
#include <variant>
#include <filesystem>

#include <Engine/Mesh.hpp>
#include <Engine/Model.hpp>
#include <Engine/Texture.hpp>
#include <Engine/Material.hpp>
#include <Engine/TextureDeserializer.hpp>

struct FNode;

struct ModelDeserializationResult {
    struct PathInfo {
        std::filesystem::path root;
        std::filesystem::path fileRelative;
    };

    bool erred{ false };
    std::vector<std::string> errors{};
    Model deserializedModel{};
    std::vector<Mesh> deserializedMeshes{};
    std::vector<size_t> meshMaterialIndices{};
    std::vector<std::variant<std::filesystem::path, TextureDeserializationResult>> deserializedTextures{};
};

ModelDeserializationResult DeserializeModel(const FNode& file) noexcept;
ModelDeserializationResult DeserializeModel(const std::string_view data, const ModelDeserializationResult::PathInfo paths = {}) noexcept;