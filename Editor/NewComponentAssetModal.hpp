#pragma once

#include <array>
#include <functional>

#include <imgui.h>

#include <Engine/FileNode.hpp>

struct GUI;

struct NewComponentAssetModal {

    explicit NewComponentAssetModal(GUI& gui) noexcept;

    void Show(FNode& currentFolder) const;
    void Hide() const;
private:
    std::reference_wrapper<GUI> gui;
    mutable FNode* currentFolder{ nullptr };
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable enum class Stage {
        /*
            1. Open a popup for component name
            2. Confirm
        */
        NameSelection, Confirmation
    } stage;

    mutable struct NewComponentAssetData {
        std::string path;
        std::string name;
    } newComponentAssetData;

    mutable std::array<char, 128> buf;
    mutable bool badName;

    void Render();
    void RenderNameSelectionDialog();
    void RenderConfirmationDialog();

    friend struct GUI;

    static constexpr auto TITLE_TEXT{ "Create a new component" };
    static constexpr auto CONTENT_TEXT{ "Please enter a name for the new component.\n" };
    static constexpr auto YES_BUTTON_TEXT{ "Yes" };
    static constexpr auto NO_BUTTON_TEXT{ "No" };
    static constexpr auto OK_BUTTON_TEXT{ "OK" };
    static constexpr auto CANCEL_BUTTON_TEXT{ "Cancel" };
    inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };
};