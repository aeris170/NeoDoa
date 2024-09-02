#include <Launcher/GUI.hpp>

#include <chrono>
#include <format>
#include <memory>
#include <algorithm>

#include <imgui.h>
#include <imgui_internal.h>
#include <stb_image.h>

#include <Submodules/detector/detector.hpp>

#include <Utility/StringTransform.hpp>
#include <Utility/Trim.hpp>

#include <Engine/TextureDeserializer.hpp>

#include <Launcher/FileDialog.hpp>

GUI::GUI(const CorePtr& core) noexcept :
    Core(core),
    Window(core->GetWindow()) {
    // Disable imgui.ini file - we don't really need to save/load imgui
    // as we dock/position everything when the program starts, leaving
    // imgui.ini obsolete.
    ImGui::GetIO().IniFilename = nullptr;
    FileDialog::Initialize();

    Window->SetTitle("NeoDoa Launcher");

    stbi_set_flip_vertically_on_load(true);
    { // Load launcher logo
        int w, h, nrChannels;
        auto* readPixels = stbi_load("Images/launcherlogo-64_x_64.png", &w, &h, &nrChannels, STBI_rgb_alpha);

        GPUTextureBuilder builder;
        builder.SetName("launcher_logo");
        if (readPixels) {
            std::span pixels{ reinterpret_cast<const std::byte*>(readPixels), w * h * nrChannels * sizeof(stbi_uc) };
            builder.SetWidth(w)
                .SetHeight(h)
                .SetData(DataFormat::RGBA8, pixels);
        } else {
            const Texture& texture = Texture::Missing();
            builder.SetWidth(texture.Width)
                .SetHeight(texture.Height)
                .SetData(texture.Format, texture.PixelData);
        }
        auto [tex, _] = builder.Build();
        launcherLogo = std::move(tex.value());

        stbi_image_free(readPixels);
    }
    {// Load vibrant launcher logo
        int w, h, nrChannels;
        auto* readPixels = stbi_load("Images/launcherlogovivid-64_x_64.png", &w, &h, &nrChannels, STBI_rgb_alpha);

        GPUTextureBuilder builder;
        builder.SetName("launcher_logo_vivid");
        if (readPixels) {
            std::span pixels{ reinterpret_cast<const std::byte*>(readPixels), w * h * nrChannels * sizeof(stbi_uc) };
            builder.SetWidth(w)
                .SetHeight(h)
                .SetData(DataFormat::RGBA8, pixels);
        } else {
            const Texture& texture = Texture::Missing();
            builder.SetWidth(texture.Width)
                .SetHeight(texture.Height)
                .SetData(texture.Format, texture.PixelData);
        }
        auto [texVivid, __] = builder.Build();
        launcherLogoVivid = std::move(texVivid.value());

        stbi_image_free(readPixels);
    }
    projectDataFile = std::make_unique<FNode>(FNodeCreationParams {
        .name = "projects"
    });

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(projectDataFile->Name().data());
    if (err == tinyxml2::XML_SUCCESS) {
        tinyxml2::XMLPrinter printer;
        doc.Print(&printer);
        ProjectDataCollectionSerializationResult result = DeserializeProjectDataCollection(printer.CStr());
        if (!result.erred) {
            projectDataCollection = std::move(result.pdc);
            isCollectionDirty = true;
        } else {
            errorModal.Show(ErrorReadingProjectData);
        }
    } else {
        if (err != tinyxml2::XML_ERROR_FILE_NOT_FOUND) {
            errorModal.Show(ErrorReadingProjectData);
        }
    }
}

void GUI::Prepare() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 1, ImGui::GetStyle().WindowPadding.y });
    window_flags = ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    dockspace_flags = ImGuiDockNodeFlags_NoCloseButton;
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }
    dockspace_flags |= ImGuiDockNodeFlags_NoTabBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    io = &ImGui::GetIO();
    font = io->Fonts->Fonts[0];
    fontBold = io->Fonts->Fonts[1];

    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize = { 800, 600 };
    style.WindowMenuButtonPosition = ImGuiDir_None;
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("NeoDoa Dockspace");

        if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
            // Clear out existing layout
            ImGui::DockBuilderRemoveNode(dockspace_id);
            // Add empty node
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            // Main node should cover entire window
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            // Build dock layout
            ImGuiID center = dockspace_id;
            ImGui::DockBuilderDockWindow("EntireAppWindow", center);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::DockSpace(dockspace_id, { 0, 0 }, dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;
}

void GUI::operator() (float delta) {
    this->delta = delta;
    Prepare();

    ImGui::PushClipRect({}, {}, false);
    ImGui::Begin("EntireAppWindow");
    RenderCustomTitleBar();
    RenderButtons();
    RenderProjectsTable();
    ImGui::End();
    ImGui::PopClipRect();

    End();
}

void GUI::End() {
    ImGui::End();
    ImGui::PopStyleVar(1);

    errorModal.Render();
    newProjectModal.Render();
    importProjectModal.Render();
}

void GUI::InsertProjectData(ProjectData&& projectData) noexcept {
    auto search = std::ranges::find_if(projectDataCollection, [&projectData](auto& elem) -> bool {
        return elem.Name == projectData.Name && elem.AbsolutePath == projectData.AbsolutePath;
    });
    if (search == projectDataCollection.end()) {
        projectDataCollection.emplace_back(std::move(projectData));
        isCollectionDirty = true;
        SaveProjectDataCollectionToDisk();
    } else if (!search->IsValid) {
        search->IsValid = true;
        search->LastOpened = "";
        isCollectionDirty = true;
        SaveProjectDataCollectionToDisk();
    } else {
        errorModal.Show(std::format(ErrorImportingAlreadyImportedProject, search->IsValid ? " and ready to launch." : "."));
    }
}

void GUI::RenderCustomTitleBar() noexcept {
    auto menuBarColor = ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg];

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImVec2 p0 = ImGui::GetCursorScreenPos() - ImGui::GetStyle().WindowPadding;
    ImVec2 p1 = { p0.x + ImGui::GetWindowWidth() , p0.y + TitleBarButtonSize.y };
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(
        p0,
        p1,
        ImGui::ColorConvertFloat4ToU32(menuBarColor)
    );

#pragma region Handle Window Dragging
    static Point windowPos{};
    static bool dragging{ false };
    if (ImGui::IsMouseHoveringRect(p0, p1) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        windowPos = Window->GetPosition();
        dragging = true;
    }
    if (dragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        dragging = false;
    }
    if (dragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto drag = ImGui::GetMouseDragDelta();
        Window->SetPosition({
            static_cast<unsigned>(windowPos.X + drag.x),
            static_cast<unsigned>(windowPos.Y + drag.y)
        });
    }
#pragma endregion

    void* logo{ nullptr };
    ImGui::SetCursorPos(TitleBarInternalPadding);
    p0 = p0 + TitleBarInternalPadding;
    ImVec2 p2 = p0 + TitleBarLogoSize;
    if (ImGui::IsMouseHoveringRect(p0, p1) || ImGui::IsMouseHoveringRect(p0, p2)) {
        logo = launcherLogoVivid;
    } else {
        logo = launcherLogo;
    }
    ImGui::Image(logo, TitleBarLogoSize);

    ImGui::SameLine();
    ImGui::Dummy(TitleBarInternalPadding);
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TitleBarInternalPadding.y / 2);
    ImGui::TextColored(TitleBarTextColor1, "Neo");
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TitleBarInternalPadding.y / 2);
    ImGui::TextColored(TitleBarTextColor2, "Doa");
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TitleBarInternalPadding.y / 2);
    ImGui::TextColored(TitleBarTextColor3, " Launcher");
    ImGui::SameLine();
    ImGui::Dummy({ ImGui::GetContentRegionAvail().x - 2 * TitleBarButtonSize.x, 1 });
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - TitleBarInternalPadding.y);

    ImGui::PushFont(fontBold);
    ImGui::PushStyleColor(ImGuiCol_Button, menuBarColor);
    auto buttonHoverColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { buttonHoverColor.x, buttonHoverColor.y, buttonHoverColor.z, 0.6f });
    if (ImGui::Button(ICON_FA_DASH, TitleBarButtonSize)) {
        Window->Minimize();
    }
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - TitleBarInternalPadding.y);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f, 0.0f, 0.0f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1.0f, 0.0f, 0.0f, 0.6f });
    if (ImGui::Button(ICON_FA_XMARK_LARGE, TitleBarButtonSize)) {
        std::exit(0);
    }
    ImGui::PopStyleColor(4);
    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - TitleBarInternalPadding.y * 2);
}
void GUI::RenderButtons() noexcept {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
    ImGui::Dummy({ ImGui::GetContentRegionAvail().x - 3 * ButtonsSize.x - 4 * ImGui::GetStyle().ItemSpacing.x - ProjectsTablePadding.x, 1 });
    ImGui::SameLine();
    if (ImGui::Button(RefreshButtonText, ButtonsSize)) {
        for (auto& data : projectDataCollection) {
            data.IsValid = CheckProjectValidity(data);
        }
    }
    ImGui::SameLine();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2{ 0, ButtonsSize.y }, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]), 1);
    ImGui::Dummy({ 0, ButtonsSize.y * 1.5f });
    ImGui::SameLine();
    if (ImGui::Button(CreateButtonText, ButtonsSize)) {
        newProjectModal.Show();
    }
    ImGui::SameLine();
    if (ImGui::Button(ImportButtonText, ButtonsSize)) {
        importProjectModal.Show();
    }
    ImGui::PopStyleVar(2);
}
void GUI::RenderProjectsTable() noexcept {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
    ImGui::Dummy(ProjectsTablePadding);
    ImGui::SameLine();
    ImGui::PopStyleVar();
    if (!projectDataCollection.empty()) {
        static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingFixedFit |
            ImGuiTableFlags_NoBordersInBodyUntilResize |
            ImGuiTableFlags_NoSavedSettings |
            ImGuiTableFlags_Reorderable |
            ImGuiTableFlags_Sortable |
            ImGuiTableFlags_SortTristate |
            ImGuiTableFlags_ScrollY;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { ImGui::GetStyle().FramePadding.x, 8 });
        if (ImGui::BeginTable("projects", 6, tableFlags, { ImGui::GetContentRegionAvail().x - ProjectsTablePadding.x, 0 })) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Project Name", ImGuiTableColumnFlags_WidthStretch, .2f);
            ImGui::TableSetupColumn("Project Path", ImGuiTableColumnFlags_WidthStretch, .4f);
            ImGui::TableSetupColumn("Last Opened", ImGuiTableColumnFlags_WidthStretch, .4f);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort);
            ImGui::TableHeadersRow();

            if (auto specs = ImGui::TableGetSortSpecs()) {
                assert(specs->SpecsCount == 0 || specs->SpecsCount == 1);
                if (specs->SpecsDirty || isCollectionDirty) {
                    projectDataCollectionSorted = projectDataCollection;
                    if (specs->SpecsCount == 1) {
                        const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
                        SortCollectionBySpec(spec);
                    }
                    specs->SpecsDirty = false;
                    isCollectionDirty = false;
                }
                if (specs->SpecsCount == 0) {
                    projectDataCollectionSorted = projectDataCollection;
                    SortCollectionByFavourite();
                }
            }

            for (auto& data : projectDataCollectionSorted) {
                RenderProjectData(data);
            }
            RenderProjectDataContextMenu(contextOwner);

            ImGui::EndTable();
        }
        ImGui::PopStyleVar(1);
    } else {
        constexpr const char* text = "Create a new project or Import an existing one.";
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text);
    }
    ImGui::PopStyleVar(2);
}
void GUI::RenderProjectData(ProjectData& data) noexcept {
    ImGui::PushID(data.AbsolutePath.c_str());
    ImVec2 min, max;
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    min = ImGui::GetCursorScreenPos();
    ImGui::AlignTextToFramePadding();
    ImGui::PushStyleColor(ImGuiCol_Text, data.IsFavourite ? ProjectsTableFavouriteColor : ProjectsTableNotFavouriteColor);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().CellPadding.x);
    if (ImGui::Button(ProjectsTableFavouriteIcon)) {
        auto search = std::ranges::find_if(projectDataCollection, [&data](auto& elem) {
            return elem.Name == data.Name && elem.AbsolutePath == data.AbsolutePath && elem.LastOpened == data.LastOpened;
        });
        assert(search != projectDataCollection.end());
        search->IsFavourite = !search->IsFavourite;
        isCollectionDirty = true;
        SaveProjectDataCollectionToDisk();
    }
    ImGui::PopStyleColor();


    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    if (data.IsValid) {
        ImGui::TextColored(ProjectsTableCheckColor, "%s", ProjectsTableCheckIcon);
    } else {
        ImGui::TextColored(ProjectsTableCrossColor, "%s", ProjectsTableCrossIcon);
    }

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(data.Name.c_str());

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(data.AbsolutePath.c_str());

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(data.LastOpened.c_str());

    ImGui::TableNextColumn();
    if (data.IsValid) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().CellPadding.x);
        if (ImGui::Button(ICON_FA_ROCKET_LAUNCH " LAUNCH!")) {
            if (CheckProjectValidity(data)) {
                auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
                auto search = std::ranges::find_if(projectDataCollection, [&data](auto& elem) {
                    return elem.Name == data.Name && elem.AbsolutePath == data.AbsolutePath && elem.LastOpened == data.LastOpened;
                });
                assert(search != projectDataCollection.end());
                if (!IsProjectAlreadyOpen(*search)) {
                    search->LastOpened = std::format("{:%Y-%m-%d %X}", time);
                    isCollectionDirty = true;
                    SaveProjectDataCollectionToDisk();

                    const char* exe;
                    if constexpr (detect::is_windows_v) {
                        exe = "start Editor.exe";
                    } else if constexpr (detect::is_linux_v) {
                        exe = "./Editor &";
                    }
                    std::string command = std::string(exe).append(1, ' ').append(data.AbsolutePath).append(data.Name).append(Assets::ProjectExtension);
                    auto sys = std::system(command.c_str());
                    if (sys == -1) {
                        DOA_LOG_WARNING("[Launcher::GUI] A call to std::system returned -1.");
                    }
                } else {
                    errorModal.Show(std::format(ErrorProjectAlreadyOpen, data.Name));
                }
            } else {
                errorModal.Show(std::format(ErrorCannotOpenProject, data.Name));
                data.IsValid = false;
            }
        }
    }

    ImGui::PopID();
    max = ImGui::GetCursorScreenPos();
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsMouseHoveringRect(min, max, false)) {
        if (!errorModal.IsVisible() && !newProjectModal.IsVisible() && !importProjectModal.IsVisible()) {
            ImGui::OpenPopup(ProjectsTableContextMenuID);
            contextOwner = data;
        }
    }
}
void GUI::RenderProjectDataContextMenu(const ProjectData& data) noexcept {
    if (ImGui::BeginPopup(ProjectsTableContextMenuID)) {
        if (ImGui::MenuItem(ProjectsTableContextMenuDeleteButtonText)) {
            auto search = std::ranges::find_if(projectDataCollection, [&data](auto& elem) {
                return elem.Name == data.Name && elem.AbsolutePath == data.AbsolutePath && elem.LastOpened == data.LastOpened;
            });
            assert(search != projectDataCollection.end());
            projectDataCollection.erase(search);
            isCollectionDirty = true;
            SaveProjectDataCollectionToDisk();
        }
        ImGui::EndPopup();
    }
}

void GUI::SaveProjectDataCollectionToDisk() noexcept {
    std::string data = SerializeProjectDataCollection(projectDataCollection);

    tinyxml2::XMLDocument doc;
    doc.Parse(data.c_str());
    doc.SaveFile(projectDataFile->Name().data());
}

void GUI::SortCollectionByFavourite() noexcept {
    std::ranges::sort(projectDataCollectionSorted, [this](auto& a, auto& b) -> bool {
        if (a.IsFavourite != b.IsFavourite) {
            return a.IsFavourite;
        }
        auto searchA = std::ranges::find_if(projectDataCollection, [&a](auto& elem) {
            return elem.Name == a.Name && elem.AbsolutePath == a.AbsolutePath && elem.LastOpened == a.LastOpened;
        });
        auto searchB = std::ranges::find_if(projectDataCollection, [&b](auto& elem) {
            return elem.Name == b.Name && elem.AbsolutePath == b.AbsolutePath && elem.LastOpened == b.LastOpened;
        });
        assert(searchA != searchB && searchA != projectDataCollection.end() && searchB != projectDataCollection.end());
        return searchA < searchB;
    });
}
void GUI::SortCollectionBySpec(const ImGuiTableColumnSortSpecs& spec) noexcept {
    if (spec.ColumnIndex == 2) { // Sort by name
        std::ranges::sort(projectDataCollectionSorted, [&spec](auto& a, auto& b) -> bool {
            if (a.IsFavourite != b.IsFavourite) {
                return a.IsFavourite;
            }

            std::string name1 = ToLowerCase(a.Name);
            std::string name2 = ToLowerCase(b.Name);
            int cmp = name1.compare(name2);
            return (spec.SortDirection == ImGuiSortDirection_Ascending)
                ? cmp < 0
                : cmp >= 0;
        });
    } else if (spec.ColumnIndex == 3) { // Sort by path
        std::ranges::sort(projectDataCollectionSorted, [&spec](auto& a, auto& b) -> bool {
            if (a.IsFavourite != b.IsFavourite) {
                return a.IsFavourite;
            }

            std::string path1 = ToLowerCase(a.AbsolutePath);
            std::string path2 = ToLowerCase(b.AbsolutePath);
            int cmp = path1.compare(path2);
            return (spec.SortDirection == ImGuiSortDirection_Ascending)
                ? cmp < 0
                : cmp >= 0;
        });
    } else if (spec.ColumnIndex == 4) { // Sort by last opened
        std::ranges::sort(projectDataCollectionSorted, [&spec](auto& a, auto& b) -> bool {
            if (a.IsFavourite != b.IsFavourite) {
                return a.IsFavourite;
            }

            std::string last1 = ToLowerCase(a.LastOpened);
            std::string last2 = ToLowerCase(b.LastOpened);
            int cmp = last1.compare(last2);
            return (spec.SortDirection == ImGuiSortDirection_Ascending)
                ? cmp < 0
                : cmp >= 0;
        });
    }
}

bool GUI::IsProjectAlreadyOpen(const ProjectData& project) noexcept {
    using namespace std::chrono_literals;
    auto maxRequestReplies{ projectDataCollection.size() };

    for (size_t i = 0; i < maxRequestReplies; i++) {
        request.Send("request_project_path", SendFlag::DontWait);
    }

    std::this_thread::sleep_for(150ms); // wait for replies to be sent.

    std::vector<std::string> openProjects{};
    for (int i = 0; i < maxRequestReplies; i++) {
        auto path = reply.Receive(ReceiveFlag::DontWait);
        if (path == "") { continue; }

        std::string s = std::string(path);
        openProjects.push_back(trim(s));
    }

    std::string absolutePath = std::string(project.AbsolutePath).append(project.Name).append(Assets::ProjectExtension);
    auto search = std::ranges::find_if(openProjects, [&absolutePath](auto& elem) {
        return elem == absolutePath;
    });
    return search != openProjects.end();
}