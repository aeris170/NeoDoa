#pragma once

#include <array>
#include <string>
#include <functional>

#include <imgui.h>

struct GUI;

struct NewProjectModal {

    explicit NewProjectModal(GUI& gui) noexcept;

    void Show() const;
    void Hide() const;

private:
    std::reference_wrapper<GUI> gui;
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable enum class Stage {
        /*
            1. Open a popup for project folder
            2. Ask for project name
            3. Confirm selections
        */
        FolderSelection, NameSelection, Confirmation
    } stage;
    mutable bool isFileDialogOpen{ false };

    mutable struct NewProjectData {
        std::string path;
        std::string name;
    } newProjectData;

    mutable std::array<char, 128> buf;
    mutable bool badName;

    void Render();
    void RenderFolderSelectionDialog();
    void RenderNameSelectionDialog();
    void RenderConfirmationDialog();

    friend struct GUI;

    static constexpr auto YES_BUTTON_TEXT{ "Yes" };
    static constexpr auto NO_BUTTON_TEXT{ "No" };
    static constexpr auto OK_BUTTON_TEXT{ "OK" };
    static constexpr auto CANCEL_BUTTON_TEXT{ "Cancel" };

    static constexpr auto TITLE_TEXT{ "Create a new Project" };

    inline static const ImVec2 MODAL_BIG_BUTTONS_SIZE{ 240, 0 };
    inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };
};