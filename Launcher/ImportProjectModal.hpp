#pragma once

#include <string>
#include <functional>

#include <imgui.h>

struct GUI;

struct ImportProjectModal {

    explicit ImportProjectModal(GUI& gui) noexcept;

    void Show() const noexcept;
    void Hide() const noexcept;
    bool IsVisible() const noexcept;

private:

    std::reference_wrapper<GUI> gui;
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable enum class Stage {
        /*
            1. Open a popup for project file
            2. Confirm project selection
        */
        Selection, Confirmation
    } stage;
    mutable bool isFileDialogOpen{ false };

    mutable struct ImportProjectData {
        std::string path;
        std::string name;
        std::string fullPath; /* path + directory separator + name */
    } importProjectData;

    void Render() noexcept;
    void RenderSelectionDialog() noexcept;
    void RenderConfirmationDialog() noexcept;

    friend struct GUI;

    static constexpr auto YES_BUTTON_TEXT{ "Yes" };
    static constexpr auto NO_BUTTON_TEXT{ "No" };

    static constexpr auto TITLE_TEXT{ "Import Project" };

    inline static const ImVec2 MODAL_BIG_BUTTONS_SIZE{ 240, 0 };
    inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };
};