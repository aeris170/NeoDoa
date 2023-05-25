#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <memory>

#include <imgui.h>

#include <Engine/Texture.hpp>

struct GUI;

struct MenuBar {

    std::reference_wrapper<GUI> gui;

    explicit MenuBar(GUI& owner);

    bool Begin();
    void Render();
    void End();

private:
    struct NewProjectModal {

        static constexpr auto MODAL_TITLE_TEXT{ "Warning!" };
        static constexpr auto MODAL_CONTENT_TEXT{ "You may have unsaved changes.\n\nAre you sure you want to create a new project?\n" };
        static constexpr auto MODAL_YES_BUTTON_TEXT{ "Yes" };
        static constexpr auto MODAL_NO_BUTTON_TEXT{ "No" };
        inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };

        std::reference_wrapper<MenuBar> mb;

        bool modal_active{ false }, modal_open{ true };
        explicit NewProjectModal(MenuBar& owner);

        void Render();
    } newProjectModal;

    struct OpenProjectModal {

        static constexpr auto MODAL_TITLE_TEXT{ "Warning!" };
        static constexpr auto MODAL_CONTENT_TEXT{ "You may have unsaved changes.\n\nAre you sure you want to open another project?\n" };
        static constexpr auto MODAL_YES_BUTTON_TEXT{ "Yes" };
        static constexpr auto MODAL_NO_BUTTON_TEXT{ "No" };
        inline static const ImVec2 MODAL_BUTTONS_SIZE{ 120, 0 };

        std::reference_wrapper<MenuBar> mb;

        bool modal_active{ false }, modal_open{ true };
        explicit OpenProjectModal(MenuBar& owner);

        void Render();
    } openProjectModal;

    struct AboutSection {

        static constexpr auto ABOUT_BUTTON_TEXT{ "About" };
        static constexpr auto ABOUT_POPUP_TITLE_TEXT{ "About" };
        static constexpr auto LIBS_BUTTON_TEXT{ "Libraries and Licences" };
        static constexpr auto LIBS_POPUP_TITLE_TEXT{ "Libraries and Licences" };
        static constexpr auto PRODUCT_NAME{ "NeoDoa Alpha" };
        static constexpr auto PRODUCT_DESCRIPTION{ "A game engine. Build: Windows" };

        std::reference_wrapper<MenuBar> mb;
        Texture neodoaBanner;
        std::vector<std::tuple<std::string, std::string>> licences;

        bool ab{ false }, ab_open{ true }, lib_open{ true };
        explicit AboutSection(MenuBar& owner);

        void RenderAboutPopup();
        void RenderLicenceNotices();
    } aboutSection;

    void RenderProjectSubMenu();
    void RenderSceneSubMenu();
    void RenderHelpSubMenu();

};

