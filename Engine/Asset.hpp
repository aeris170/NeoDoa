#pragma once

#include "UUID.hpp"
#include "FileNode.hpp"

#include "Scene.hpp"
#include "Component.hpp"
//#include "Script.hpp"
#include "Texture.hpp"
#include "Model.hpp"
//#include "Material.hpp"
#include "Shader.hpp"

#include "TemplateUtilities.hpp"

#include <variant>

#define ASSET_TYPE Scene, Component, Texture/*, Model, Shader */
template<typename T>
concept AssetType = concepts::IsAnyOf<T, ASSET_TYPE> && concepts::Copyable<T> && concepts::Serializable<T> && std::movable<T>;
using AssetData = std::variant<std::monostate, ASSET_TYPE>;
#undef ASSET_TYPE

struct Asset {

    Asset() noexcept;
    Asset(const UUID id, FNode* file) noexcept;
    ~Asset() = default;
    Asset(const Asset& other) = default;
    Asset(Asset&& other) noexcept;
    Asset& operator=(const Asset& other) = default;
    Asset& operator=(Asset&& other) noexcept;

    UUID ID() const;
    FNode& File() const;
    const AssetData& Data() const;
    template<AssetType T>
    T& DataAs() {
        return std::get<T>(data);
    }

    void Serialize();
    void Deserialize();
    void ForceDeserialize();
    void DeleteDeserializedData();
    bool HasDeserializedData() const;

    UUID Instantiate() const;

    template<AssetType T>
    void UpdateData(T&& newData) {
        data = std::move(newData);
    }

    bool IsScene() const;
    bool IsComponentDefinition() const;
    bool IsScript() const;
    bool IsTexture() const;
    bool IsModel() const;
    bool IsMaterial() const;
    bool IsShader() const;

    bool HasInfoMessages() const;
    const std::vector<std::any>& InfoMessages() const;

    bool HasWarningMessages() const;
    const std::vector<std::any>& WarningMessages() const;

    bool HasErrorMessages() const;
    const std::vector<std::any>& ErrorMessages() const;

private:
    UUID id {};
    FNode* file { nullptr };
    AssetData data{ std::monostate{} };

    std::vector<std::any> infoList{};
    std::vector<std::any> warningList{};
    std::vector<std::any> errorList {};
};
