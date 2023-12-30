#pragma once

#include <string>
#include <filesystem>
#include <cstdlib>

#include <entt/entt.hpp>

#include "UUID.hpp"
#include "Asset.hpp"
#include "FileNode.hpp"

struct AssetHandle {

    AssetHandle() noexcept;
    AssetHandle(Asset* const asset) noexcept;
    Asset& operator*() const noexcept;
    Asset* operator->() const noexcept;
    operator Asset* () const noexcept;
    operator bool() const noexcept;

    bool HasValue() const;
    Asset& Value() const;
    void Reset();

    friend bool operator==(const AssetHandle& handle1, const AssetHandle& handle2) = default;

private:
    Asset* _asset;
};

struct Assets {

    using UUIDCollection = std::vector<UUID>;

    inline static std::string PROJ_EXT{ ".doa" };
    inline static std::string SCENE_EXT{ ".scn" };
    inline static std::string SCRIPT_EXT{ ".scrpt" };
    inline static std::string TEXTURE_EXT_PNG{ ".png" };
    inline static std::string TEXTURE_EXT_JPG{ ".jpg" };
    inline static std::string TEXTURE_EXT_JPEG{ ".jpeg" };
    inline static std::string MODEL_EXT{ ".mdl" };
    inline static std::string MATERIAL_EXT{ ".mat" };
    inline static std::string VERTEX_SHADER_EXT{ ".vert" };
    inline static std::string TESS_CTRL_SHADER_EXT{ ".tesc" };
    inline static std::string TESS_EVAL_SHADER_EXT{ ".tese" };
    inline static std::string GEOMETRY_SHADER_EXT{ ".geom" };
    inline static std::string FRAGMENT_SHADER_EXT{ ".frag" };
    inline static std::string COMPUTE_SHADER_EXT{ ".comp" };
    inline static std::string SHADER_PROGRAM_EXT{ ".prog" };
    inline static std::string COMP_EXT{ ".ncd" };
    inline static std::string ID_EXT{ ".id" };

    static bool IsSceneFile(const FNode& file);
    static bool IsScriptFile(const FNode& file);
    static bool IsTextureFile(const FNode& file);
    static bool IsModelFile(const FNode& file);
    static bool IsMaterialFile(const FNode& file);
    static bool IsShaderFile(const FNode& file);
    static bool IsVertexShaderFile(const FNode& file);
    static bool IsTessellationControlShaderFile(const FNode& file);
    static bool IsTessellationEvaluationShaderFile(const FNode& file);
    static bool IsGeometryShaderFile(const FNode& file);
    static bool IsFragmentShaderFile(const FNode& file);
    static bool IsComputeShaderFile(const FNode& file);
    static bool IsShaderProgramFile(const FNode& file);
    static bool IsComponentDefinitionFile(const FNode& file);

    explicit Assets(const Project& project) noexcept;
    ~Assets() = default;
    Assets(const Assets&) = delete;
    Assets(Assets&&) = delete;
    Assets& operator=(const Assets&) = delete;
    Assets& operator=(Assets&&) = delete;

    FNode& CreateFolder(FNode& parentFolder, const std::string_view folderName);
    void MoveFolder(FNode& folder, FNode& targetParentFolder);
    void DeleteFolder(FNode& folder);

    template<AssetType T>
    AssetHandle CreateAssetAt(FNode& folderPath, const std::string_view fileName, const std::string_view serializedData) {
        std::filesystem::path p = fileName;
        const FNode* newAssetFile = folderPath.CreateChildFile({
            folderPath.owner,
            &folderPath,
            p.stem().string(),
            p.extension().string(),
            std::string(serializedData)
        });
        AssetHandle rv = ImportFile(database, *newAssetFile);
        rv->ForceDeserialize();
        return rv;
    }
    void SaveAsset(const AssetHandle asset);
    void MoveAsset(const AssetHandle asset, FNode& targetParentFolder);
    void DeleteAsset(const AssetHandle asset);

    AssetHandle FindAsset(UUID uuid) const;
    AssetHandle FindAssetAt(const FNode& file) const;
    bool IsAssetExistsAt(const FNode& file) const;

    FNode& Root();
    const FNode& Root() const;

    const UUIDCollection& SceneAssetIDs() const;
    const UUIDCollection& ScriptAssetIDs() const;
    const UUIDCollection& TextureAssetIDs() const;
    const UUIDCollection& ComponentDefinitionAssetIDs() const;
    const UUIDCollection& ModelAssetIDs() const;
    const UUIDCollection& ShaderAssetIDs() const;
    const UUIDCollection& ShaderProgramAssetIDs() const;
    const UUIDCollection& ShaderUniformBlockAssetIDs() const;

    AssetHandle Import(const FNode& file);
    void ReimportAll();

    void EnsureDeserialization();

private:

#if _DEBUG
    using AssetDatabase = std::unordered_map<UUID, Asset>;
    using AssetFileDatabase = std::unordered_map<const FNode*, UUID>;
#elif _NDEBUG
    using AssetDatabase = entt::dense_map<UUID, Asset>;
    using AssetFileDatabase = entt::dense_map<const FNode*, UUID>;
#endif

    AssetDatabase database{};
    AssetFileDatabase files{};

    FNode _root;

    UUIDCollection sceneAssets{};
    UUIDCollection scriptAssets{};
    UUIDCollection textureAssets{};
    UUIDCollection componentDefinitionAssets{};
    UUIDCollection modelAssets{};
    UUIDCollection shaderAssets{};
    UUIDCollection shaderProgramAssets{};
    UUIDCollection shaderUniformBlockAssets{};

    AssetHandle ImportFile(AssetDatabase& database, const FNode& file);
    void ImportAllFiles(AssetDatabase& database, const FNode& root);
    void Deserialize(const UUIDCollection& assets);

    void BuildFileNodeTree(const Project& project, FNode& root);
};
