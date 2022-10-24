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
    operator Asset* () const;
    operator bool() const;

    bool HasValue() const;
    Asset& Value() const;
    void Reset();

private:
    Asset* _asset;
};

struct Assets {

    using UUIDCollection = std::vector<UUID>;
    using AssetFileDatabase = entt::dense_hash_map<const FNode*, UUID>;

    inline static std::string SCENE_EXT{ ".scn" };
    inline static std::string SCRIPT_EXT{ ".scrpt" };
    inline static std::string TEXTURE_EXT{ ".png" };
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

    FNode& CreateFolder(FNode& parentFolder, const std::string_view folderName);
    void MoveFolder(FNode& folder, FNode& targetParentFolder);
    void DeleteFolder(FNode& folder);

    template<AssetType T, typename ...Args>
    AssetHandle CreateAssetAt(FNode& folderPath, const std::string_view fileName, Args&& ...args) {
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
    void MoveAsset(const AssetHandle asset, FNode& targetParentFolder);
    void DeleteAsset(const AssetHandle asset);

    AssetHandle FindAsset(UUID uuid);
    const AssetHandle FindAsset(UUID uuid) const;
    AssetHandle FindAssetAt(const FNode& file);
    const AssetHandle FindAssetAt(const FNode& file) const;
    bool IsAssetExistsAt(const FNode& file) const;

    FNode& Root();
    const FNode& Root() const;

    const UUIDCollection& SceneAssetIDs() const;
    const UUIDCollection& ScriptAssetIDs() const;
    const UUIDCollection& TextureAssetIDs() const;
    const UUIDCollection& ModelAssetIDs() const;
    const UUIDCollection& ShaderAssetIDs() const;
    const UUIDCollection& ShaderUniformBlockAssetIDs() const;

    void ReimportAll();

private:

    using AssetDatabase = entt::dense_hash_map<UUID, Asset>;

    const Project* project{ nullptr };
    AssetDatabase database{};
    AssetFileDatabase files{};

    FNode _root;

    UUIDCollection sceneAssets{};
    UUIDCollection scriptAssets{};
    UUIDCollection textureAssets{};
    UUIDCollection modelAssets{};
    UUIDCollection shaderAssets{};
    UUIDCollection shaderUniformBlockAssets{};

    AssetHandle ImportFile(AssetDatabase& database, const FNode& file);
    void ImportAllFiles(AssetDatabase& database, const FNode& root);

    void BuildFileNodeTree(FNode& root);

    friend struct Project;

    inline void __pointersInvalidated(Project* newOwner) {
        project = newOwner;
        _root.__pointersInvalidated(newOwner);
    }
};
