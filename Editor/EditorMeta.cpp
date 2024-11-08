#include <Editor/EditorMeta.hpp>

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

void EditorMeta::SaveImGuiIniSettingsToDisk() const noexcept {
    auto current = std::filesystem::current_path();
    std::filesystem::current_path(MetaFolderName);
    ImGui::SaveIniSettingsToDisk(ImGuiSettingsFileName);
    std::filesystem::current_path(current);
}

void EditorMeta::CreateMetaAssetInfoBank() noexcept {
    metaAssetInfoBank = std::make_unique<MetaAssetInfoBank>();
    MetaAssetInfoBank::LoadFromDisk(*metaAssetInfoBank.get(), *editorMetaFolder);
}

void EditorMeta::OnProjectLoaded([[maybe_unused]] const Project& project) noexcept {
    static const auto& Core = Core::GetCore();
    const auto& assets = Core->GetAssets();
    CreateHiddenMetaDataFolderIfNotExists(assets->Root());
    CreateMetaAssetInfoBank();

    auto current = std::filesystem::current_path();
    std::filesystem::current_path(MetaFolderName);
    ImGui::LoadIniSettingsFromDisk(ImGuiSettingsFileName);
    std::filesystem::current_path(current);
}
void EditorMeta::OnProjectSaved([[maybe_unused]] const Project& project) noexcept {
    MetaAssetInfoBank::SaveToDisk(*metaAssetInfoBank.get(), *editorMetaFolder);
}
void EditorMeta::OnProjectUnloaded() noexcept {
    SaveImGuiIniSettingsToDisk();

    editorMetaFolder = nullptr;
    metaAssetInfoBank = nullptr;
}

#ifdef _WIN64
// Do not move this include! Windows.h defines a lot of junk and it seeps into other .h files, causing lots of bizarre errors.
// Exclude rarely-used stuff from Windows headers and in this case prevent compiler errors from redefinition of UUID
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
void EditorMeta::CreateHiddenMetaDataFolderIfNotExists(FNode& root) noexcept {
    FNode* folder = root.CreateChildFolderIfNotExists({
        .name = EditorMeta::MetaFolderName
    });
    if (folder == nullptr) {
        editorMetaFolder = &root.FindChild(std::filesystem::path(EditorMeta::MetaFolderName));
    } else {
        editorMetaFolder = folder;
    }
#ifdef _WIN64
    auto path = editorMetaFolder->AbsolutePath().string();
    SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif
}
