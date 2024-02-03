#include <Editor/EditorMetaInitializer.hpp>

#ifdef _WIN64
// Exclude rarely-used stuff from Windows headers and in this case prevent compiler errors from redefinition of UUID
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <Submodules/detector/detector.hpp>

#include <Engine/FileNode.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/UserDefinedComponentStorageSerializer.hpp>
#include <Editor/UserDefinedComponentStorageDeserializer.hpp>

static GUI* EditorGUI{ nullptr };
static MetaAssetInfoBank* metaAssetInfoBank{ nullptr };
static FNode* EditorMetaFolder{ nullptr };

void InitializeEditorMeta(GUI& gui) noexcept {
    assert(!EditorGUI); // forgot call  to DestoryEditorMeta before InitializeEditorMeta
    EditorGUI = &gui;
    gui.Events.OnProjectLoaded += []([[maybe_unused]] const Project& project) {
        static const auto& Core = Core::GetCore();
        CreateHiddenMetaDataFolderIfNotExists(Core->Assets()->Root());
        metaAssetInfo->
    };
    gui.Events.OnProjectUnloaded += []() { EditorMetaFolder = nullptr; };

    SceneSerializer::Entities::SerializeUserDefinedComponents = SerializeUserDefinedComponentStorage;
    SceneDeserializer::Entities::DeserializeUserDefinedComponents = DeserializeUserDefinedComponentStorage;
}

FNode& GetEditorMetaFolder() noexcept {
    assert(EditorMetaFolder); // Oops, you were about to d-ref a nullptr! Good thing I crashed it before.
    return *EditorMetaFolder;
}
void CreateHiddenMetaDataFolderIfNotExists(FNode& root) noexcept {
    FNode* folder = root.CreateChildFolderIfNotExists({
        .name = ".metadata"
    });
    if (folder == nullptr) {
        EditorMetaFolder = &root.FindChild(std::filesystem::path(".metadata"));
    } else {
        EditorMetaFolder = folder;
    }

    if constexpr (detect::is_windows_v) {
        auto path = EditorMetaFolder->AbsolutePath().string();
        SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
}

void DestoryEditorMeta() noexcept {
    EditorGUI = nullptr;
}