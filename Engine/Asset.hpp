#pragma once

#include "UUID.hpp"
#include "FileNode.hpp"

#include "Scene.hpp"
//#include "Script.hpp"
#include "Texture.hpp"
#include "Model.hpp"
//#include "Material.hpp"
#include "Shader.hpp"

#include <variant>
#include <concepts>


namespace detail {
#define ASSET_TYPE Scene, Texture, Model, Shader

	template<typename T, typename ...U>
	concept IsAnyOf = (std::same_as<T, U> || ...);

	template<typename T>
	concept Copyable = requires(const T& t) {
		{ T::Copy(t) } -> std::same_as<T>;
	};

	template<typename T>
	concept Serializable = requires {
		&T::Serialize;
		&T::Deserialize;
	};

	template<typename T>
	concept AssetType = IsAnyOf<T, ASSET_TYPE> && Copyable<T> && Serializable<T> && std::movable<T>;
}

using AssetData = std::variant<std::monostate, ASSET_TYPE>;
#undef ASSET_TYPE

struct Asset {

	Asset() noexcept;
	Asset(const UUID id, const FNode* file) noexcept;
	~Asset() = default;
	Asset(const Asset& other) = default;
	Asset(Asset&& other) noexcept;
	Asset& operator=(const Asset& other) = default;
	Asset& operator=(Asset&& other) noexcept;

	UUID ID() const;
	const FNode* File() const;
	const AssetData& Data() const;
	template<detail::AssetType T>
	T& DataAs() {
		return std::get<T>(data);
	}

	void Serialize();
	void Deserialize();
	void ForceDeserialize();
	void DeleteDeserializedData();

	UUID Instantiate() const;

	template<detail::AssetType T>
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
	const FNode* file;
	AssetData data{ std::monostate{} };
};
