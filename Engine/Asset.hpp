#pragma once

#include <any>
#include <variant>

#include <Utility/ObserverPattern.hpp>
#include <Utility/TemplateUtilities.hpp>

#include <Engine/UUID.hpp>
#include <Engine/FileNode.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Component.hpp>
#include <Engine/Sampler.hpp>
#include <Engine/Texture.hpp>
#include <Engine/Shader.hpp>
#include <Engine/Material.hpp>
#include <Engine/FrameBuffer.hpp>
//#include "Script.hpp"
//#include "Model.hpp"


#define ASSET_TYPE Scene, Component, Sampler, Texture, Shader, ShaderProgram, Material, FrameBuffer/*, Model*/
template<typename T>
concept AssetType = concepts::IsAnyOf<T, ASSET_TYPE> && concepts::Copyable<T> && concepts::Serializable<T> && std::movable<T>;
using AssetData = std::variant<std::monostate, ASSET_TYPE>;
#undef ASSET_TYPE

struct Asset final : ObserverPattern::Observable {

    Asset() noexcept;
    Asset(const UUID id, FNode* file) noexcept;
    ~Asset() noexcept override;
    Asset(const Asset& other) = delete;
    Asset(Asset&& other) noexcept;
    Asset& operator=(const Asset& other) = delete;
    Asset& operator=(Asset&& other) noexcept;

    UUID ID() const;
    FNode& File() const;
    const AssetData& Data() const;
    template<AssetType T>
    T& DataAs() {
        return std::get<T>(data);
    }
    template<AssetType T>
    const T& DataAs() const {
        return std::get<T>(data);
    }
    uint64_t Version() const;

    void Serialize();
    void Deserialize();
    void ForceDeserialize();
    void DeleteDeserializedData();
    bool HasDeserializedData() const;

    UUID Instantiate() const;

    template<AssetType T>
    void UpdateData(T&& newData) {
        data = std::forward<T>(newData);
    }

    bool IsScene() const;
    bool IsComponentDefinition() const;
    bool IsSampler() const;
    bool IsTexture() const;
    bool IsShader() const;
    bool IsShaderProgram() const;
    bool IsMaterial() const;
    bool IsFrameBuffer() const;
    bool IsScript() const;
    bool IsModel() const;

    bool HasInfoMessages() const;
    const std::vector<std::any>& InfoMessages() const;

    bool HasWarningMessages() const;
    const std::vector<std::any>& WarningMessages() const;

    bool HasErrorMessages() const;
    const std::vector<std::any>& ErrorMessages() const;

protected:
    void NotifyObservers(ObserverPattern::Notification message) final;

private:
    UUID id{ UUID::Empty() };
    FNode* file{ nullptr };
    AssetData data{ std::monostate{} };
    uint64_t version{};

    std::vector<std::any> infoList{};
    std::vector<std::any> warningList{};
    std::vector<std::any> errorList{};
};
