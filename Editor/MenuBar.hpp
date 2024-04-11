#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <imgui.h>

#include <Engine/GPUTexture.hpp>

struct GUI;

struct MenuBar {

    std::reference_wrapper<GUI> gui;

    explicit MenuBar(GUI& owner) noexcept;

    bool Begin() noexcept;
    void Render() noexcept;
    void End() noexcept;

private:

    struct AboutSection {

        static constexpr auto ABOUT_BUTTON_TEXT{ "About" };
        static constexpr auto ABOUT_POPUP_TITLE_TEXT{ "About" };
        static constexpr auto LIBS_BUTTON_TEXT{ "Libraries and Licences" };
        static constexpr auto LIBS_POPUP_TITLE_TEXT{ "Libraries and Licences" };
        static constexpr auto PRODUCT_NAME{ "NeoDoa Alpha" };
        static constexpr auto PRODUCT_DESCRIPTION{ "A game engine. Build: Windows" };

        std::reference_wrapper<MenuBar> mb;
        GPUTexture neodoaBanner;
        std::vector<std::tuple<std::string, std::string>> licences;

        bool ab{ false }, ab_open{ true }, lib_open{ true };
        explicit AboutSection(MenuBar& owner) noexcept;

        void RenderAboutPopup() noexcept;
        void RenderLicenceNotices() noexcept;
    } aboutSection;

    void RenderFileSubMenu() noexcept;
    void RenderEditSubMenu() noexcept;
    void RenderAssetsSubMenu() noexcept;
    void RenderHelpSubMenu() noexcept;

};

