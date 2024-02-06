#pragma once

#include <memory>

#include <Engine/Assets.hpp>
#include <Engine/Project.hpp>
#include <Engine/FileNode.hpp>

struct GUI;
struct MetaAssetInfoBank;

struct EditorMeta {
    explicit EditorMeta(GUI& gui) noexcept;

    GUI& GetEditorGUI() const noexcept;
    FNode& GetEditorMetaFolder() const noexcept;
    MetaAssetInfoBank& GetMetaAssetInfoBank() const noexcept;

private:
    static constexpr auto MetaFolderName{ ".metadata" };

    std::reference_wrapper<GUI> editorGUI;
    FNode* editorMetaFolder{ nullptr };
    std::unique_ptr<MetaAssetInfoBank> metaAssetInfoBank{ nullptr };

    void CreateMetaAssetInfoBank() noexcept;

    void OnProjectLoaded(const Project& project) noexcept;
    void OnProjectSaved(const Project& project) noexcept;
    void OnProjectUnloaded() noexcept;

    void CreateHiddenMetaDataFolderIfNotExists(FNode& root) noexcept;
};
