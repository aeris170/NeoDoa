#pragma once

#include <array>
#include <functional>

#include <imgui.h>

#include <Engine/Asset.hpp>
#include <Engine/Shader.hpp>
#include <Engine/FileNode.hpp>

struct GUI;

struct NewAssetModal {

    explicit NewAssetModal(GUI& gui) noexcept;

    void ShowSceneCreationModal(FNode& currentFolder) const;
    void ShowComponentCreationModal(FNode& currentFolder) const;
    void ShowVertexShaderCreationModal(FNode& currentFolder) const;
    void ShowTessellationControlShaderCreationModal(FNode& currentFolder) const;
    void ShowTessellationEvaluationShaderCreationModal(FNode& currentFolder) const;
    void ShowGeometryShaderCreationModal(FNode& currentFolder) const;
    void ShowFragmentShaderCreationModal(FNode& currentFolder) const;
    void ShowShaderProgramCreationModal(FNode& currentFolder) const;

    void Hide() const;
private:
    std::reference_wrapper<GUI> gui;
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable enum class Stage {
        /*
            1. Open a popup for component name
            2. Confirm
        */
        NameSelection, Confirmation
    } stage;
    mutable std::array<char, 128> buf;
    mutable bool badName;
    mutable std::string titleText;
    mutable std::string confirmText;

    mutable struct NewAssetData {
        enum class AssetType {
            Scene,
            Component,
            VertexShader,
            TessellationControlShader,
            TessellationEvaluationShader,
            GeometryShader,
            FragmentShader,
            ShaderProgram,
            Texture,
            Model
        } type;
        FNode* currentFolder{ nullptr };
        std::string path;
        std::string name;
    } newAssetData;

    void Reset(FNode& currentFolder, NewAssetData::AssetType typeOfAssetToCreate);
    void Render();
    void RenderNameSelectionDialog();
    void RenderConfirmationDialog();

    void CreateAsset();
    void CreateSceneAsset();
    void CreateComponentAsset();
    void CreateVertexShaderAsset();
    void CreateTessellationControlAsset();
    void CreateTessellationEvaluationAsset();
    void CreateGeometryShaderAsset();
    void CreateFragmentShaderAsset();
    void CreateShaderProgramAsset();

    friend struct GUI;

    static constexpr auto TITLE_TEXT{ "Create a new {}" };
    static constexpr auto CONFIRM_TEXT{ "Continue and create {} asset?" };
    static constexpr auto YES_BUTTON_TEXT{ "Yes" };
    static constexpr auto NO_BUTTON_TEXT{ "No" };
    static constexpr auto OK_BUTTON_TEXT{ "OK" };
    static constexpr auto CANCEL_BUTTON_TEXT{ "Cancel" };
    inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };
};