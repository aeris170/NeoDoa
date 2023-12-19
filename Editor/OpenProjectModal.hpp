#pragma once

#include <string>
#include <functional>

#include <imgui.h>

struct GUI;

struct OpenProjectModal {

    explicit OpenProjectModal(GUI& gui) noexcept;

    void Show() const;
    void Hide() const;

private:

    std::reference_wrapper<GUI> gui;
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable enum class Stage {
        /*
            1. Warn user if they have unsaved changes
            2. Open a popup for project file
            4. Confirm project selection
        */
        Warning, Selection, Confirmation
    } stage;
    mutable bool isFileDialogOpen{ false };

    mutable struct OpenProjectData {
        std::string path;
        std::string name;
        std::string fullPath; /* path + directory separator + name */
    } openProjectData;

    void Render();
    void RenderWarningDialog();
    void RenderSelectionDialog();
    void RenderConfirmationDialog();

    friend struct GUI;

    static constexpr auto WARNING_TITLE_TEXT{ "Warning!" };
    static constexpr auto WARNING_TEXT{ "You have unsaved changes.\n\nAre you sure you want to open another project?\n" };
    static constexpr auto CONT_NO_SAVE_BUTTON_TEXT{ "Continue without saving" };
    static constexpr auto CONT_SAVE_BUTTON_TEXT{ "Save & Continue" };
    static constexpr auto YES_BUTTON_TEXT{ "Yes" };
    static constexpr auto NO_BUTTON_TEXT{ "No" };

    static constexpr auto TITLE_TEXT{ "Open Project" };

    inline static const ImVec2 MODAL_BIG_BUTTONS_SIZE{ 240, 0 };
    inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };
};