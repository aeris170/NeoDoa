#pragma once

#include <array>
#include <format>
#include <functional>

#include <imgui.h>

#include <Engine/Shader.hpp>

struct GUI;
struct Assets;
struct FNode;
struct Project;

struct AssetManager {

    static constexpr auto REFRESH_BUTTON_TEXT{ "Refresh" };
    static constexpr auto SELECTED_FOLDER_CONTENT_TITLE_TEXT{ "Assets" };

    std::reference_wrapper<GUI> gui;

    explicit AssetManager(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:
    bool hasContent{ false };
    Assets* assets{ nullptr };
    FNode* root{ nullptr };
    FNode* selectedNode{ nullptr };
    FNode* currentFolder{ nullptr };
    FNode* deletedNode{ nullptr };
    UUID dragDropAssetID{};

    struct TreeViewSettings {
        float currentWidth = 120.0f;
        float minWidth = 1.0f;
        int selectedElemIndex = 0;
    } treeViewSettings{};

    struct CurrentFolderContentSettings {
        float currentWidth = 300.0f;
        float minWidth = 120.0f;

        float itemPadding = 16.0f;
        float thumbnailSize = 64.0f;
        const float thumbnailMinSize = 48.0f;
        const float thumbnailMaxSize = 96.0f;
        int maxTextLine = 3;
        enum class ViewMode {
            Icons,
            List
        } viewMode = ViewMode::Icons;
    } currentFolderContentSettings{};

    struct FileFilter {
        std::reference_wrapper<AssetManager> Owner;
        std::array<char, 32> SearchQuery{};
        bool CheckVisibility(const FNode& file) const;
    } fileFilter{ *this };

    struct NewShaderModal {

        using OnCreateNewShader = std::function<void(const FNode& file)>;

        static constexpr auto VERTEX_SHADER_MODAL_TITLE_TEXT    { "Create a new Vertex Shader" };
        static constexpr auto TESS_CTRL_SHADER_MODAL_TITLE_TEXT { "Create a new Tessellation Control Shader" };
        static constexpr auto TESS_EVAL_SHADER_MODAL_TITLE_TEXT { "Create a new Tessellation Evaluation Shader" };
        static constexpr auto GEOMETRY_SHADER_MODAL_TITLE_TEXT  { "Create a new Geometry Shader" };
        static constexpr auto FRAGMENT_SHADER_MODAL_TITLE_TEXT  { "Create a new Fragment Shader" };
        static constexpr auto COMPUTE_SHADER_MODAL_TITLE_TEXT   { "Create a new Compute Shader" };

        static constexpr auto VERTEX_SHADER_MODAL_CONTENT_TEXT    { "Please enter a name for the new Vertex Shader.\n" };
        static constexpr auto TESS_CTRL_SHADER_MODAL_CONTENT_TEXT { "Please enter a name for the new Tessellation Control Shader.\n" };
        static constexpr auto TESS_EVAL_SHADER_MODAL_CONTENT_TEXT { "Please enter a name for the new Tessellation Evaluation Shader.\n" };
        static constexpr auto GEOMETRY_SHADER_MODAL_CONTENT_TEXT  { "Please enter a name for the new Geometry Shader.\n" };
        static constexpr auto FRAGMENT_SHADER_MODAL_CONTENT_TEXT  { "Please enter a name for the new Fragment Shader.\n" };
        static constexpr auto COMPUTE_SHADER_MODAL_CONTENT_TEXT   { "Please enter a name for the new Compute Shader.\n" };

        static constexpr auto MODAL_BUTTON_TEXT{ "OK" };
        inline static const ImVec2 MODAL_BUTTON_SIZE{ 120, 0 };

        void Activate(FNode* currentFolder, enum class Shader::Type shaderType, OnCreateNewShader callback = {});
        void Render();

    private:
        FNode* currentFolder{ nullptr };
        enum class Shader::Type shaderType{};
        OnCreateNewShader callback{};
        bool modal_active{ false }, modal_open{ true };
        char shaderName[128]{ "MyShader" }; /* we dont need to incur a dyn-alloc with std::string */

        static const char* const& SelectModalTitle(enum class Shader::Type shaderType);
        static const char* const& SelectModalContent(enum class Shader::Type shaderType);
    } newShaderModal{};

    struct NewShaderProgramModal {

        using OnCreateNewProgram = std::function<void(const FNode& file)>;

        static constexpr auto SHADER_PROGRAM_MODAL_TITLE_TEXT{ "Create a new Shader Program" };

        static constexpr auto SHADER_PROGRAM_MODAL_CONTENT_TEXT{ "Please enter a name for the new Shader Program.\n" };

        static constexpr auto MODAL_BUTTON_TEXT{ "OK" };
        inline static const ImVec2 MODAL_BUTTON_SIZE{ 120, 0 };

        void Activate(FNode* currentFolder, OnCreateNewProgram callback = {});
        void Render();

    private:
        FNode* currentFolder{ nullptr };
        OnCreateNewProgram callback{};
        bool modal_active{ false }, modal_open{ true };
        char programName[128]{ "MyProgram" }; /* we dont need to incur a dyn-alloc with std::string */
    } newShaderProgramModal{};

    void RenderMenuBar();
    void RenderTreeView();
    void RenderTreeViewRecursive(FNode& current);

    void RenderSelectedFolderContent();
    void RenderContextMenu();
    void OpenFileAtFileNode(FNode& file);

    void SetSelectedNode(FNode* node);
    void SetCurrentFolder(FNode* folder);

    void OnProjectLoaded(Project& project);
    void OnProjectUnloaded();
};

