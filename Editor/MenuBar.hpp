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

