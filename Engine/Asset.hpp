#pragma once

#include "UUID.hpp"
#include "FileNode.hpp"

#include "Scene.hpp"
//#include "Script.hpp"
#include "Texture.hpp"
#include "Model.hpp"
//#include "Material.hpp"
#include "Shader.hpp"

#include "TemplateUtilities.hpp"

#include <variant>

#define ASSET_TYPE Scene, Texture, Model, Shader
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
	FNode* File() const;
	const AssetData& Data() const;
	template<AssetType T>
	T& DataAs() {
		return std::get<T>(data);
	}

	void Serialize();
	void Deserialize();
	void ForceDeserialize();
	void DeleteDeserializedData();

	UUID Instantiate() const;

	template<AssetType T>
	void UpdateData(T&& newData) {
		data = std::move(newData);
	}

	bool IsScene() const;
	bool IsScript() const;
	bool IsTexture() const;
	bool IsModel() const;
	bool IsMaterial() const;
	bool IsShader() const;

private:
	UUID id;
	FNode* file;
	AssetData data{ std::monostate{} };
};
