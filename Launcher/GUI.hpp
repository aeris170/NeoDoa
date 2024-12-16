#pragma once

#include <string>
#include <vector>

#include <tinyxml2.h>

#include <Utility/SimpleSocket.hpp>

#include <Engine/Core.hpp>
#include <Engine/FileNode.hpp>
#include <Engine/GPUTexture.hpp>
#include <Engine/FontAwesome.hpp>

#include <Launcher/ProjectData.hpp>

#include <Launcher/ErrorModal.hpp>
#include <Launcher/NewProjectModal.hpp>
#include <Launcher/ImportProjectModal.hpp>

struct GUI {

    const CorePtr& Core;
    const std::unique_ptr<IWindow>& Window;

    explicit GUI(const CorePtr& core) noexcept;

    void Prepare();
    void operator() (float delta);
    void End();

    void InsertProjectData(ProjectData&& args) noexcept;

private:
    float delta;

    GPUTexture launcherLogo{};
    GPUTexture launcherLogoVivid{};

    ImGuiWindowFlags window_flags{ ImGuiWindowFlags_None };
    ImGuiDockNodeFlags dockspace_flags{ ImGuiDockNodeFlags_None };
    bool dockspaceOpen{ true };

    ImGuiIO* io{ nullptr };
    ImFont* font{ nullptr };
    ImFont* fontBold{ nullptr };

    ErrorModal errorModal{ *this };
    NewProjectModal newProjectModal{ *this };
    ImportProjectModal importProjectModal{ *this };

    std::unique_ptr<FNode> projectDataFile{};
    ProjectDataCollection projectDataCollection{};

    bool isCollectionDirty{ false };
    ProjectDataCollection projectDataCollectionSorted{};

    void RenderCustomTitleBar() noexcept;
    void RenderButtons() noexcept;
    void RenderProjectsTable() noexcept;
    void RenderProjectData(ProjectDataCollection& projectDataColectionSorted) noexcept;

    void SaveProjectDataCollectionToDisk() noexcept;

    void SortCollectionByFavourite() noexcept;
    void SortCollectionBySpec(const ImGuiTableColumnSortSpecs& spec) noexcept;

    //- Sockets -//
    ServerSocket request{ "tcp://*:5555", SocketType::Push };
    ServerSocket reply{ "tcp://*:5556", SocketType::Pull };
    bool IsProjectAlreadyOpen(const ProjectData& project) noexcept;
    //- Sockets -//

    //- Error Modal Text Constants -//
    static constexpr std::string_view ErrorReadingProjectData{ "An error occured while reading projects data.\nPlease re-import your existing projects.\n\n" };
    static constexpr std::string_view ErrorImportingAlreadyImportedProject{ "The project you are trying to import is already imported{}" };
    static constexpr std::string_view ErrorCannotOpenProject{ "Project {} couldn't be opened. It is either corrupted or moved." };
    static constexpr std::string_view ErrorProjectAlreadyOpen{ "Project {} is already open." };
    //- Error Modal Text Constants -//

    //- Title Bar Constants -//
    inline static ImVec2 TitleBarLogoSize{ 64, 64 };
    inline static ImVec2 TitleBarButtonSize{ 64, 36 };
    inline static ImVec2 TitleBarInternalPadding{ 6, 6 };
    inline static ImVec4 TitleBarTextColor1{ 0.2f, 0.7f, 0.2f, 1.0f };
    inline static ImVec4 TitleBarTextColor2{ 0.94f, 0.36f, 0.95f, 1.0f };
    inline static ImVec4 TitleBarTextColor3{ 1.0f, 1.0f, 1.0f, 1.0f };
    //- Title Bar Constants -//

    //- Buttons Constants -//
    inline static ImVec2 ButtonsSize{ 64, 32 };
    inline static const char* RefreshButtonText{ ICON_FA_ROTATE };
    inline static const char* CreateButtonText{ ICON_FA_PLUS_LARGE };
    inline static const char* ImportButtonText{ ICON_FA_FOLDER_OPEN };
    //- Buttons Constants -//

    //- Projects Table Constants -//
    inline static ImVec2 ProjectsTablePadding{ 12, 6 };
    inline static ImVec4 ProjectsTableFavouriteColor{ 1, 1, 0, 1 };
    inline static ImVec4 ProjectsTableNotFavouriteColor{ 1, 1, 1, 0.6f };
    inline static ImVec4 ProjectsTableCheckColor{ 0, 1, 0, 1};
    inline static ImVec4 ProjectsTableCrossColor{ 1, 0, 0, 1 };
    inline static const char* ProjectsTableFavouriteIcon{ ICON_FA_STAR };
    inline static const char* ProjectsTableCheckIcon{ ICON_FA_CHECK };
    inline static const char* ProjectsTableCrossIcon{ ICON_FA_XMARK_LARGE };

    inline static const char* ProjectsTableContextMenuID{ "context_menu" };
    inline static const char* ProjectsTableContextMenuDeleteButtonText{ "  " ICON_FA_TRASH " Delete" };
    //- Projects Table Constants -//
};