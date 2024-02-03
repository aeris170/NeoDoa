#include <Editor/EditorMeta.hpp>

#include <Submodules/detector/detector.hpp>

#include <Engine/FileNode.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/UserDefinedComponentStorageSerializer.hpp>
#include <Editor/UserDefinedComponentStorageDeserializer.hpp>

EditorMeta::EditorMeta(GUI& gui) noexcept :
    EditorGUI(gui) {
    SceneSerializer::Entities::SerializeUserDefinedComponents = SerializeUserDefinedComponentStorage;
    SceneDeserializer::Entities::DeserializeUserDefinedComponents = DeserializeUserDefinedComponentStorage;

    gui.Events.OnProjectLoaded   += std::bind_front(&EditorMeta::OnProjectLoaded,   this);
    gui.Events.OnProjectSaved    += std::bind_front(&EditorMeta::OnProjectSaved,    this);
    gui.Events.OnProjectUnloaded += std::bind_front(&EditorMeta::OnProjectUnloaded, this);
}

GUI& EditorMeta::GetEditorGUI() const noexcept { return EditorGUI; }
FNode& EditorMeta::GetEditorMetaFolder() const noexcept {
    assert(EditorMetaFolder); // Oops, was about to d-ref a nullptr! Good thing I crashed it before.
    return *EditorMetaFolder;
}
MetaAssetInfoBank& EditorMeta::GetMetaAssetInfoBank() const noexcept {
    assert(MetaAssetInfoBank); // Oops, was about to d-ref a nullptr! Good thing I crashed it before.
    return *MetaAssetInfoBank;
}

#ifdef _WIN64
// Do not move this include! Windows.h defined a lot of junk and it seeps into other .h files, causing lots of bizarre errors.
// Exclude rarely-used stuff from Windows headers and in this case prevent compiler errors from redefinition of UUID
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
template<Platform platform = GetCurrentPlatform()>
void CreateHiddenMetaDataFolderIfNotExists(EditorMeta& meta, FNode& root) noexcept {
    FNode* folder = root.CreateChildFolderIfNotExists({
        .name = EditorMeta::MetaFolderName
    });
    if (folder == nullptr) {
        meta.EditorMetaFolder = &root.FindChild(std::filesystem::path(EditorMeta::MetaFolderName));
    } else {
        meta.EditorMetaFolder = folder;
    }

    if constexpr (platform == Platform::Windows) {
        auto path = meta.EditorMetaFolder->AbsolutePath().string();
        SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
}
void EditorMeta::CreateMetaAssetInfoBank() noexcept {
    MetaAssetInfoBank = std::make_unique<struct MetaAssetInfoBank>();
    MetaAssetInfoBank::LoadFromDisk(*MetaAssetInfoBank.get(), *EditorMetaFolder);
}

void EditorMeta::OnProjectLoaded(const Project& project) noexcept {
    static const auto& Core = Core::GetCore();
    const auto& assets = Core->Assets();
    CreateHiddenMetaDataFolderIfNotExists(*this, assets->Root());
    CreateMetaAssetInfoBank();
}
void EditorMeta::OnProjectSaved(const Project& project) noexcept {
    MetaAssetInfoBank::SaveToDisk(*MetaAssetInfoBank.get(), *EditorMetaFolder);
}
void EditorMeta::OnProjectUnloaded() noexcept {
    EditorMetaFolder = nullptr;
    MetaAssetInfoBank = nullptr;
}