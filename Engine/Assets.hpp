#pragma once

#include <any>
#include <string>
#include <cstdlib>
#include <variant>
#include <filesystem>
#include <unordered_map>

#include <entt/entt.hpp>

#include <Utility/AdjacencyList.hpp>
#include <Utility/TemplateUtilities.hpp>

#include <Engine/Log.hpp>
#include <Engine/UUID.hpp>
#include <Engine/Event.hpp>
#include <Engine/FileNode.hpp>

// In order asset types
#include <Engine/Scene.hpp>
#include <Engine/Component.hpp>
#include <Engine/Sampler.hpp>
#include <Engine/Texture.hpp>
#include <Engine/Shader.hpp>
#include <Engine/Material.hpp>
#include <Engine/FrameBuffer.hpp>
//#include "Script.hpp"
//#include "Model.hpp"

struct Assets;
struct AssetGPUBridge;

#define ASSET_TYPE Scene, Component, Sampler, Texture, Shader, ShaderProgram, Material, FrameBuffer/*, Model*/
template<typename T>
concept AssetType = concepts::IsAnyOf<T, ASSET_TYPE>&& concepts::Copyable<T>&& concepts::Serializable<T>&& std::movable<T>;
using AssetData = std::variant<std::monostate, ASSET_TYPE>;
#undef ASSET_TYPE

struct Asset final {

    Asset(const UUID id, Assets& owningManager) noexcept;
    ~Asset() noexcept;
    Asset(const Asset& other) = delete;
    Asset(Asset&& other) noexcept;
    Asset& operator=(const Asset& other) = delete;
    Asset& operator=(Asset&& other) noexcept;

    UUID ID() const;
    FNode& File() const;
    const AssetData& Data() const;
    template<AssetType T>
    T& DataAs();
    template<AssetType T>
    const T& DataAs() const;
    uint64_t Version() const;

    void Serialize();
    void Deserialize();
    void ForceDeserialize();
    void DeleteDeserializedData();
    bool HasDeserializedData() const;

    UUID Instantiate() const;

    template<AssetType T>
    void UpdateData(T&& newData);

    bool IsScene() const noexcept;
    bool IsComponentDefinition() const noexcept;
    bool IsSampler() const noexcept;
    bool IsTexture() const noexcept;
    bool IsShader() const noexcept;
    bool IsShaderProgram() const noexcept;
    bool IsMaterial() const noexcept;
    bool IsFrameBuffer() const noexcept;
    bool IsScript() const noexcept;
    bool IsModel() const noexcept;

    bool HasInfoMessages() const;
    const std::vector<std::any>& InfoMessages() const;

    bool HasWarningMessages() const;
    const std::vector<std::any>& WarningMessages() const;

    bool HasErrorMessages() const;
    const std::vector<std::any>& ErrorMessages() const;

private:
    UUID id{ UUID::Empty() };
    std::reference_wrapper<Assets> owningManager;
};

// Encapsulates Asset*. AssetHandle's are fundamentally unsafe to
// hold onto. DON'T cache AssetHandle objects as they are no different
// than pointers. The underlying pointer is susceptible to dangling as
// additions/removals may shift where the actual data is stored. If you
// need to keep track of an Asset, cache it's UUID and access via.
// Handles are best used in one-and-done situations where you won't need
// continuous access to the data.
struct AssetHandle {

    AssetHandle() noexcept;
    AssetHandle(Asset* const asset) noexcept;
    Asset& operator*() const noexcept;
    Asset* operator->() const noexcept;
    operator Asset*() const noexcept;
    operator bool() const noexcept;

    bool HasValue() const noexcept;
    Asset& Value() const noexcept;
    void Reset() noexcept;

    friend bool operator==(const AssetHandle& handle1, const AssetHandle& handle2) = default;

private:
    Asset* _asset;
};

/* Data-Oriented Asset Database Layout */
struct AssetDatabase {
#if DEBUG
    template<typename T>
    using UUIDMap = std::unordered_map<UUID, T>;
#elif NDEBUG
    template<typename T>
    using UUIDMap = entt::dense_map<UUID, T>;
#else
#error "Neither DEBUG nor NDEBUG are defined!"
#endif

    Asset& operator[](size_t index) noexcept;
    const Asset& operator[](size_t index) const noexcept;
    UUIDMap<Asset>::iterator begin() noexcept;
    UUIDMap<Asset>::iterator end() noexcept;
    UUIDMap<Asset>::const_iterator cbegin() const noexcept;
    UUIDMap<Asset>::const_iterator cend() const noexcept;
    UUIDMap<Asset>::const_iterator begin() const noexcept;
    UUIDMap<Asset>::const_iterator end() const noexcept;

    Asset& Emplace(UUID uuid, FNode* file, Assets& owningManager) noexcept;
    bool Contains(UUID uuid) const noexcept;
    Asset& At(UUID uuid) noexcept;
    const Asset& At(UUID uuid) const noexcept;
    void Remove(UUID uuid) noexcept;
    void Clear() noexcept;

private:
    AssetDatabase() noexcept = default;

    // Asset doesn't directly hold any data anymore.
    // It exists to merely present an OOP-esque
    // usage to NeoDoa Asset Management System.
    UUIDMap<Asset> assets{};
    UUIDMap<FNode*> files{};
    UUIDMap<AssetData> data{};
    UUIDMap<uint64_t> versions{};
    UUIDMap<std::vector<std::any>> infoLists{};
    UUIDMap<std::vector<std::any>> warningLists{};
    UUIDMap<std::vector<std::any>> errorLists{};

    friend struct Assets;
};
/* Data-Oriented Asset Database Layout */

struct Assets {

    struct Events {
        Event<void(const UUID uuid)> OnAssetDeserialized{};
        Event<void(const UUID uuid)> OnAssetDataDeleted{};
        Event<void(const UUID uuid)> OnAssetDestructed{};
    } Events{};

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

    FNode& CreateFolder(FNode& parentFolder, const std::string_view folderName) noexcept;
    void MoveFolder(FNode& folder, FNode& targetParentFolder) noexcept;
    void DeleteFolder(FNode& folder) noexcept;

    AssetHandle CreateAssetAt(FNode& folderPath, const std::string_view fileName, const std::string_view serializedData) noexcept;
    void MoveAsset(const UUID uuid, FNode& targetParentFolder) noexcept;
    void DeleteAsset(const UUID uuid) noexcept;

    AssetHandle FindAsset(const UUID uuid) const noexcept;
    AssetHandle FindAssetAt(const FNode& file) const noexcept;
    bool IsAssetExistsAt(const FNode& file) const noexcept;

    FNode& GetFileOfAsset(const UUID uuid) const noexcept;
    const AssetData& GetDataOfAsset(const UUID uuid) const noexcept;
    template<AssetType T>
    T& GetDataOfAssetAs(const UUID uuid) noexcept {
        assert(database.data.contains(uuid));
        return std::get<T>(database.data[uuid]);
    }
    template<AssetType T>
    const T& GetDataOfAssetAs(const UUID uuid) const noexcept {
        assert(database.data.contains(uuid));
        return std::get<T>((*const_cast<AssetDatabase::UUIDMap<AssetData>*>(&database.data))[uuid]);
    }
    uint64_t GetVersionOfAsset(const UUID uuid) const noexcept;

    void SerializeAsset(const UUID uuid) noexcept;
    void DeserializeAsset(const UUID uuid) noexcept;
    void ForceDeserializeAsset(const UUID uuid) noexcept;
    void DeleteDeserializedDataOfAsset(const UUID uuid) noexcept;
    bool AssetHasDeserializedData(const UUID uuid) const noexcept;
    UUID InstantiateAsset(const UUID uuid) const noexcept;

    template<AssetType T>
    void UpdateDataOfAsset(const UUID uuid, T&& newData) noexcept {
        database.data[uuid] = std::forward<T>(newData);
    }

    bool IsSceneAsset(const UUID uuid) const noexcept;
    bool IsComponentDefinitionAsset(const UUID uuid) const noexcept;
    bool IsSamplerAsset(const UUID uuid) const noexcept;
    bool IsTextureAsset(const UUID uuid) const noexcept;
    bool IsShaderAsset(const UUID uuid) const noexcept;
    bool IsShaderProgramAsset(const UUID uuid) const noexcept;
    bool IsMaterialAsset(const UUID uuid) const noexcept;
    bool IsFrameBufferAsset(const UUID uuid) const noexcept;
    bool IsScriptAsset(const UUID uuid) const noexcept;
    bool IsModelAsset(const UUID uuid) const noexcept;

    bool AssetHasInfoMessages(const UUID uuid) const noexcept;
    const std::vector<std::any>& GetInfoMessagesOfAsset(const UUID uuid) const noexcept;

    bool AssetHasWarningMessages(const UUID uuid) const noexcept;
    const std::vector<std::any>& GetWarningMessagesOfAsset(const UUID uuid) const noexcept;

    bool AssetHasErrorMessages(const UUID uuid) const noexcept;
    const std::vector<std::any>& GetErrorMessagesOfAsset(const UUID uuid) const noexcept;

    FNode& Root() noexcept;
    const FNode& Root() const noexcept;

    const UUIDCollection& AllAssetsIDs() const noexcept;
    const UUIDCollection& SceneAssetIDs() const noexcept;
    const UUIDCollection& ScriptAssetIDs() const noexcept;
    const UUIDCollection& SamplerAssetIDs() const noexcept;
    const UUIDCollection& TextureAssetIDs() const noexcept;
    const UUIDCollection& ComponentDefinitionAssetIDs() const noexcept;
    const UUIDCollection& ModelAssetIDs() const noexcept;
    const UUIDCollection& ShaderAssetIDs() const noexcept;
    const UUIDCollection& ShaderProgramAssetIDs() const noexcept;
    const UUIDCollection& MaterialAssetIDs() const noexcept;
    const UUIDCollection& FrameBufferAssetIDs() const noexcept;

    const AssetGPUBridge& GPUBridge() const noexcept;

    std::pair<UUID, AssetHandle> Import(const FNode& file) noexcept;
    void ReimportAll() noexcept;

    void EnsureDeserialization() noexcept;

    void TryRegisterDependencyBetween(UUID dependent, UUID dependency) noexcept;
    void TryDeleteDependencyBetween(UUID dependent, UUID dependency) noexcept;

private:

#if DEBUG
    using AssetFileDatabase = std::unordered_map<const FNode*, UUID>;
#elif NDEBUG
    using AssetFileDatabase = entt::dense_map<const FNode*, UUID>;
#else
#error "Neither DEBUG nor NDEBUG are defined!"
#endif

    /* Object-Oriented Asset Database Layout - OLD */
    //using AssetDatabase = std::unordered_map<UUID, Asset>; // DEBUG
    //using AssetDatabase = entt::dense_map<UUID, Asset>; // NDEBUG
    //AssetDatabase database{};
    /* Object-Oriented Asset Database Layout - OLD */

    /* Data-Oriented Asset Database Layout */
    AssetDatabase database{};
    /* Data-Oriented Asset Database Layout */

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

    std::pair<UUID, AssetHandle> ImportFile(AssetDatabase& database, const FNode& file) noexcept;
    void ImportAllFiles(AssetDatabase& database, const FNode& root) noexcept;
    void Deserialize(const UUIDCollection& assets) noexcept;

    void BuildFileNodeTree(const Project& project, FNode& root) noexcept;
    void ReBuildDependencyGraph() noexcept;

    void DeleteOtherResourcesCorrespondingToDeletedAsset(const UUID uuid) noexcept;

    template<AssetType T>
    void PerformPostDeserializationAction([[maybe_unused]] const UUID uuid) noexcept {}
};

template<>
void Assets::PerformPostDeserializationAction<Sampler>(const UUID uuid) noexcept;
template<>
void Assets::PerformPostDeserializationAction<Texture>(const UUID uuid) noexcept;
template<>
void Assets::PerformPostDeserializationAction<Shader>(const UUID uuid) noexcept;
template<>
void Assets::PerformPostDeserializationAction<ShaderProgram>(const UUID uuid) noexcept;

// Material Post Serialiation Helpers
namespace MaterialPostDeserialization {
    size_t TypeNameToVariantIndex(std::string_view typeName) noexcept;
    void InsertUniform(Material::Uniforms& uniforms, int location, const UniformValue& uniform) noexcept;
    void EmplaceUniform(Material::Uniforms& uniforms, int location, std::string_view name, std::string_view typeName, int arraySize = 1) noexcept;
}

template<>
void Assets::PerformPostDeserializationAction<Material>(const UUID uuid) noexcept;
template<>
void Assets::PerformPostDeserializationAction<FrameBuffer>(const UUID uuid) noexcept;

template<AssetType T>
T& Asset::DataAs() {
    return owningManager.get().GetDataOfAssetAs<T>(id);
}
template<AssetType T>
const T& Asset::DataAs() const {
    return owningManager.get().GetDataOfAssetAs<T>(id);
}

template<AssetType T>
void Asset::UpdateData(T&& newData) {
    return owningManager.get().UpdateDataOfAsset(id, std::forward<T>(newData));
}