#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <memory>

struct GUI;
struct ImGuiIO;
struct ImFont;
struct Texture;

struct MenuBar {

	GUI* const gui;

	MenuBar(GUI* owner);

	void Begin();
	void Render();
	void End();

private:
	struct AboutSection {

		static constexpr const char* const ABOUT_BUTTON_TEXT{ "About" };
		static constexpr const char* const ABOUT_POPUP_TITLE_TEXT{ "About" };
		static constexpr const char* const LIBS_BUTTON_TEXT{ "Libraries and Licences" };
		static constexpr const char* const LIBS_POPUP_TITLE_TEXT{ "Libraries and Licences" };
		static constexpr const char* const PRODUCT_NAME{ "NeoDoa Alpha" };
		static constexpr const char* const PRODUCT_DESCRIPTION{ "A game engine. Build: Windows" };

		MenuBar& mb;
		std::weak_ptr<Texture> neodoaBanner;
		std::vector<std::tuple<std::string, std::string>> licences;

		bool ab{ false }, ab_open{ true }, lib_open{ true };
		AboutSection(MenuBar& owner);

		void RenderAboutPopup();
		void RenderLicenceNotices();
	} aboutSection;

	void RenderProjectSubMenu();
	void RenderSceneSubMenu();
	void RenderHelpSubMenu();

};
