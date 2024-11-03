#pragma once

#include <string>
#include <cstdlib>
#include <filesystem>
#include <unordered_map>

#include <entt/entt.hpp>

#include <Utility/AdjacencyList.hpp>
#include <Utility/ObserverPattern.hpp>

#include <Engine/Log.hpp>
#include <Engine/UUID.hpp>
#include <Engine/Asset.hpp>
#include <Engine/FileNode.hpp>

struct AssetGPUBridge;

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

struct Assets : ObserverPattern::Observer {

    using UUIDCollection = std::vector<UUID>;

    inline static std::string ProjectExtension{ ".doa" };
    inline static std::string SceneExtension{ ".scn" };
    inline static std::string ComponentDefinitionExtension{ ".ncd" };
    inline static std::string SamplerExtension{ ".smplr" };
    inline static std::string TextureExtensionPNG{ ".png" };
    inline static std::string TextureExtensionBMP{ ".bmp" };
    inline static std::string TextureExtensionTGA{ ".tga" };
    inline static std::string TextureExtensionJPG{ ".jpg" };
    inline static std::string TextureExtensionJPEG{ ".jpeg" };
    inline static std::string VertexShaderExtension{ ".vert" };
    inline static std::string TessellationControlShaderExtension{ ".tesc" };
    inline static std::string TessellationEvaluationShaderExtension{ ".tese" };
    inline static std::string GeometryShaderExtension{ ".geom" };
    inline static std::string FragmentShaderExtension{ ".frag" };
    inline static std::string ComputeShaderExtension{ ".comp" };
    inline static std::string ShaderProgramExtension{ ".prog" };
    inline static std::string MaterialExtension{ ".mat" };
    inline static std::string FrameBufferExtension{ ".fbo" };
    inline static std::string SCRIPT_EXT{ ".scrpt" };
    inline static std::string MODEL_EXT{ ".mdl" };
    inline static std::string AssetIDExtension{ ".id" };

    static bool IsProjectFile(const FNode& file) noexcept;
    static bool IsSceneFile(const FNode& file) noexcept;
    static bool IsComponentDefinitionFile(const FNode& file) noexcept;
    static bool IsSamplerFile(const FNode& file) noexcept;
    static bool IsTextureFile(const FNode& file) noexcept;
    static bool IsShaderFile(const FNode& file) noexcept;
    static bool IsVertexShaderFile(const FNode& file) noexcept;
    static bool IsTessellationControlShaderFile(const FNode& file) noexcept;
    static bool IsTessellationEvaluationShaderFile(const FNode& file) noexcept;
    static bool IsGeometryShaderFile(const FNode& file) noexcept;
    static bool IsFragmentShaderFile(const FNode& file) noexcept;
    static bool IsComputeShaderFile(const FNode& file) noexcept;
    static bool IsShaderProgramFile(const FNode& file) noexcept;
    static bool IsMaterialFile(const FNode& file) noexcept;
    static bool IsFrameBufferFile(const FNode& file) noexcept;
    static bool IsScriptFile(const FNode& file) noexcept;
    static bool IsModelFile(const FNode& file) noexcept;

    explicit Assets(const Project& project, AssetGPUBridge& bridge) noexcept;
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

    const UUIDCollection& AllAssetsIDs() const;
    const UUIDCollection& SceneAssetIDs() const;
    const UUIDCollection& ScriptAssetIDs() const;
    const UUIDCollection& SamplerAssetIDs() const;
    const UUIDCollection& TextureAssetIDs() const;
    const UUIDCollection& ComponentDefinitionAssetIDs() const;
    const UUIDCollection& ModelAssetIDs() const;
    const UUIDCollection& ShaderAssetIDs() const;
    const UUIDCollection& ShaderProgramAssetIDs() const;
    const UUIDCollection& MaterialAssetIDs() const;
    const UUIDCollection& FrameBufferAssetIDs() const;

    const AssetGPUBridge& GPUBridge() const;

    AssetHandle Import(const FNode& file);
    void ReimportAll();

    void EnsureDeserialization();

    void TryRegisterDependencyBetween(UUID dependent, UUID dependency) noexcept;
    void TryDeleteDependencyBetween(UUID dependent, UUID dependency) noexcept;

protected:
    void OnNotify(const ObserverPattern::Observable* source, ObserverPattern::Notification message) final;

private:

#if DEBUG
    using AssetDatabase = std::unordered_map<UUID, Asset>;
    using AssetFileDatabase = std::unordered_map<const FNode*, UUID>;
#elif NDEBUG
    using AssetDatabase = entt::dense_map<UUID, Asset>;
    using AssetFileDatabase = entt::dense_map<const FNode*, UUID>;
#else
#error "Neither DEBUG nor NDEBUG are defined!"
#endif

    AssetDatabase database{};
    AssetFileDatabase files{};

    FNode _root;

    UUIDCollection allAssets{};
    UUIDCollection sceneAssets{};
    UUIDCollection scriptAssets{};
    UUIDCollection samplerAssets{};
    UUIDCollection textureAssets{};
    UUIDCollection componentDefinitionAssets{};
    UUIDCollection modelAssets{};
    UUIDCollection shaderAssets{};
    UUIDCollection shaderProgramAssets{};
    UUIDCollection materialAssets{};
    UUIDCollection frameBufferAssets{};

    AdjacencyList<UUID> dependencyGraph{};

    AssetGPUBridge& bridge;

    AssetHandle ImportFile(AssetDatabase& database, const FNode& file);
    void ImportAllFiles(AssetDatabase& database, const FNode& root);
    void Deserialize(const UUIDCollection& assets);

    void BuildFileNodeTree(const Project& project, FNode& root);
    void ReBuildDependencyGraph() noexcept;

    template<AssetType T>
    void PerformPostDeserializationAction([[maybe_unused]] UUID id) noexcept {}
};

template<>
void Assets::PerformPostDeserializationAction<Sampler>(UUID id) noexcept;
template<>
void Assets::PerformPostDeserializationAction<Texture>(UUID id) noexcept;
template<>
void Assets::PerformPostDeserializationAction<Shader>(UUID id) noexcept;
template<>
void Assets::PerformPostDeserializationAction<ShaderProgram>(UUID id) noexcept;

// Material Post Serialiation Helpers
namespace MaterialPostDeserialization {
    size_t TypeNameToVariantIndex(std::string_view typeName) noexcept;
    void InsertUniform(Material::Uniforms& uniforms, int location, const UniformValue& uniform) noexcept;
    void EmplaceUniform(Material::Uniforms& uniforms, int location, std::string_view name, std::string_view typeName, int arraySize = 1) noexcept;
}

template<>
void Assets::PerformPostDeserializationAction<Material>(UUID id) noexcept;
template<>
void Assets::PerformPostDeserializationAction<FrameBuffer>(UUID id) noexcept;