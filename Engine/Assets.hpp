#pragma once

#include <string>
#include <filesystem>
#include <cstdlib>

#include <entt.hpp>

#include "UUID.hpp"
#include "Asset.hpp"
#include "FileNode.hpp"

struct Project;

struct AssetHandle {

	AssetHandle() noexcept;
	AssetHandle(Asset* const asset) noexcept;
	~AssetHandle() = default;
	AssetHandle(const AssetHandle& other) = default;
	AssetHandle(AssetHandle&& other) = default;
	AssetHandle& operator=(const AssetHandle& other) = default;
	AssetHandle& operator=(AssetHandle&& other) = default;
	Asset& operator*() const;
	Asset* operator->() const;
	operator Asset*() const;
	operator bool() const;

	bool HasValue() const;
	Asset& Value() const;
	void Reset();

private:
	Asset* _asset;
};

struct Assets {

	using AssetDatabaseCategory = std::vector<UUID>;
	using AssetFileDatabase = entt::dense_hash_map<const FNode*, UUID>;

	inline static std::string SCENE_EXT{ ".scn" };
	inline static std::string SCRIPT_EXT{ ".scrpt" };
	inline static std::string TEXTURE_EXT{ ".tex" };
	inline static std::string MODEL_EXT{ ".mdl" };
	inline static std::string MATERIAL_EXT{ ".mat" };
	inline static std::string SHADER_EXT{ ".shdr" };
	inline static std::string ID_EXT{ ".id" };

	static bool IsSceneFile(const FNode* file);
	static bool IsScriptFile(const FNode* file);
	static bool IsTextureFile(const FNode* file);
	static bool IsModelFile(const FNode* file);
	static bool IsMaterialFile(const FNode* file);
	static bool IsShaderFile(const FNode* file);

	Assets(const Project* owner) noexcept;
	~Assets() = default;
	Assets(const Assets& other) = delete;
	Assets(Assets&& other) noexcept;
	Assets& operator=(const Assets& other) = delete;
	Assets& operator=(Assets&& other) noexcept;

	bool CreateFolder(std::filesystem::path relativePath);
	bool MoveFolder(std::filesystem::path oldRelativePath, std::filesystem::path newRelativePath);
	bool DeleteFolder(std::filesystem::path relativePath);

	template<detail::AssetType T, typename ...Args>
	AssetHandle CreateAsset(std::filesystem::path relativePath, Args&& ...args) {
		/*
		std::filesystem::current_path(project->Workspace());
		FNode* folder = _root.FindChildAt(relativePath.parent_path());
		if (folder == null) return std::nullopt;
		FNode* newAssetFile = folder->CreateChildFileFor(folder,
			{
				folder->owner,
				folder,
				relativePath.stem(),
				relativePath.extension(),
				T(std::forward<Args>(args)...).Serialize()
			}
		);
		ImportFile(database, *newAssetFile);
		*/
		return nullptr;
	}
	bool MoveAsset(std::filesystem::path oldRelativePath, std::filesystem::path newRelativePath);
	bool DeleteAsset(std::filesystem::path relativePath);

	AssetHandle FindAsset(UUID uuid);
	AssetHandle FindAsset(std::filesystem::path relativePath);

	bool IsAsset(FNode* file) const;

	FNode& Root();
	const FNode& Root() const;

	const AssetDatabaseCategory& SceneAssets() const;
	const AssetDatabaseCategory& ScriptAssets() const;
	const AssetDatabaseCategory& TextureAssets() const;
	const AssetDatabaseCategory& ModelAssets() const;
	const AssetDatabaseCategory& ShaderAssets() const;
	const AssetDatabaseCategory& ShaderUniformBlockAssets() const;

private:

	using AssetDatabase = entt::dense_hash_map<UUID, Asset>;

	const Project* project{ nullptr };
	AssetDatabase database{};
	AssetFileDatabase files{};

	FNode _root;

	AssetDatabaseCategory sceneAssets{};
	AssetDatabaseCategory scriptAssets{};
	AssetDatabaseCategory textureAssets{};
	AssetDatabaseCategory modelAssets{};
	AssetDatabaseCategory shaderAssets{};
	AssetDatabaseCategory shaderUniformBlockAssets{};

	AssetHandle ImportFile(AssetDatabase& database, const FNode& file);
	void ImportAllFiles(AssetDatabase& database, const FNode& root);

	void BuildFileNodeTree(FNode& root);

	friend struct Project;

	inline void __pointersInvalidated(Project* newOwner) {
		project = newOwner;
		_root.__pointersInvalidated(newOwner);
	}
};
