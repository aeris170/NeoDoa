#pragma once

#include <Engine/Model.hpp>
#include <Engine/Assets.hpp>

struct Observer;

struct ModelDisplay {

    explicit ModelDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(const AssetHandle modelAssetHandle) noexcept;
    void RenderMessagesTable() noexcept;
    void RenderModelContents() noexcept;
    void DrawTreeVisualization(const Tree<Model::Node>& tree) noexcept;

private:
    std::reference_wrapper<Observer> observer;

    AssetHandle modelAsset{};

    void RenderMeshInfo(std::span<const Model::Mesh> meshes, std::span<const std::string> meshNames, std::span<const UUID> meshUUIDs) noexcept;

    void RenderTextureInfo(std::span<const Model::Texture> textures) noexcept;

    void RenderMaterialInfo(std::span<const Model::Material> materials) noexcept;
    void RenderMaterialColorInfo(std::string_view name, const std::optional<Color>& color) noexcept;
    void RenderMaterialFloatInfo(std::string_view name, const std::optional<float>& floatValue) noexcept;
    void RenderMaterialBoolInfo(std::string_view name, const std::optional<bool>& boolValue) noexcept;
    void RenderMaterialShadingModeInfo(std::string_view name, const std::optional<Model::Material::ShadingMode>& shading) noexcept;
    void RenderMaterialBlendModeInfo(std::string_view name, const std::optional<Model::Material::BlendMode>& blend) noexcept;
    void RenderMaterialTextureStack(const Model::Material::TextureStack& stack) noexcept;
    void RenderMaterialTextureInfo(const Model::Material::TextureInfo& info) noexcept;

    void OnAssetRefreshed(AssetHandle handle) noexcept;
};
