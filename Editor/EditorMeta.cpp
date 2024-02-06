#include <Editor/EditorMeta.hpp>

#include <Submodules/detector/detector.hpp>

#include <Engine/FileNode.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/UserDefinedComponentStorageSerializer.hpp>
#include <Editor/UserDefinedComponentStorageDeserializer.hpp>

EditorMeta::EditorMeta(GUI& gui) noexcept :
    editorGUI(gui) {
    SceneSerializer::Entities::SerializeUserDefinedComponents = SerializeUserDefinedComponentStorage;
    SceneDeserializer::Entities::DeserializeUserDefinedComponents = DeserializeUserDefinedComponentStorage;

    gui.Events.OnProjectLoaded   += std::bind_front(&EditorMeta::OnProjectLoaded,   this);
    gui.Events.OnProjectSaved    += std::bind_front(&EditorMeta::OnProjectSaved,    this);
    gui.Events.OnProjectUnloaded += std::bind_front(&EditorMeta::OnProjectUnloaded, this);
}

GUI& EditorMeta::GetEditorGUI() const noexcept { return editorGUI; }
FNode& EditorMeta::GetEditorMetaFolder() const noexcept {
    assert(editorMetaFolder); // Oops, was about to d-ref a nullptr! Good thing I crashed it before.
    return *editorMetaFolder;
}
MetaAssetInfoBank& EditorMeta::GetMetaAssetInfoBank() const noexcept {
    assert(metaAssetInfoBank); // Oops, was about to d-ref a nullptr! Good thing I crashed it before.
    return *metaAssetInfoBank;
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
        meta.editorMetaFolder = &root.FindChild(std::filesystem::path(EditorMeta::MetaFolderName));
    } else {
        meta.editorMetaFolder = folder;
    }

    if constexpr (platform == Platform::Windows) {
        auto path = meta.editorMetaFolder->AbsolutePath().string();
        SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
}
void EditorMeta::CreateMetaAssetInfoBank() noexcept {
    metaAssetInfoBank = std::make_unique<MetaAssetInfoBank>();
    MetaAssetInfoBank::LoadFromDisk(*metaAssetInfoBank.get(), *editorMetaFolder);
}

void EditorMeta::OnProjectLoaded(const Project& project) noexcept {
    static const auto& Core = Core::GetCore();
    const auto& assets = Core->GetAssets();
    CreateHiddenMetaDataFolderIfNotExists(*this, assets->Root());
    CreateMetaAssetInfoBank();
}
void EditorMeta::OnProjectSaved(const Project& project) noexcept {
    MetaAssetInfoBank::SaveToDisk(*metaAssetInfoBank.get(), *editorMetaFolder);
}
void EditorMeta::OnProjectUnloaded() noexcept {
    editorMetaFolder = nullptr;
    metaAssetInfoBank = nullptr;
}