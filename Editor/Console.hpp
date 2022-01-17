#pragma once

#include <imgui.h>
#include <IconsFontAwesome5Pro.h>

#include <Log.hpp>

struct GUI;

struct Console {

	static constexpr const char* const TRACE_ICON{ ICON_FA_INFO };
	static constexpr const char* const INFO_ICON{ ICON_FA_INFO };
	static constexpr const char* const WARNING_ICON{ ICON_FA_EXCLAMATION_CIRCLE };
	static constexpr const char* const ERROR_ICON{ ICON_FA_EXCLAMATION_TRIANGLE };
	static constexpr const char* const FATAL_ICON{ ICON_FA_EXCLAMATION_TRIANGLE };
	static constexpr const char* const OPENGL_ICON{ ICON_FA_SERVER };
	static constexpr const char* const VULKAN_ICON{ ICON_FA_SERVER };
	static constexpr const char* const DIRECTX_ICON{ ICON_FA_SERVER };

	static const ImVec4 TRACE_COLOR;
	static const ImVec4 INFO_COLOR;
	static const ImVec4 WARNING_COLOR;
	static const ImVec4 ERROR_COLOR;
	static const ImVec4 FATAL_COLOR;
	static const ImVec4 OPENGL_COLOR;
	static const ImVec4 VULKAN_COLOR;
	static const ImVec4 DIRECTX_COLOR;

	static constexpr const char* const TRACE_TOOLTIP_MESSAGE{ "General purpose hints/clues" };
	static constexpr const char* const INFO_TOOLTIP_MESSAGE{ "General notices" };
	static constexpr const char* const WARNING_TOOLTIP_MESSAGE{ "Explanations to potentially unwanted situations" };
	static constexpr const char* const ERROR_TOOLTIP_MESSAGE{ "Expected(!), but potentially very fragile situations. These will crash your game if you ship it with these :)" };
	static constexpr const char* const FATAL_TOOLTIP_MESSAGE{ "Unexpected behavior" };
	static constexpr const char* const OPENGL_TOOLTIP_MESSAGE{ "OpenGL Server message" };
	static constexpr const char* const VULKAN_TOOLTIP_MESSAGE{ "Vulkan Server message" };
	static constexpr const char* const DIRECTX_TOOLTIP_MESSAGE{ "Direct-X Server message" };

	static constexpr const char* const TRACE_BUTTON_TOOLTIP_MESSAGE{ "Show all messages" };
	static constexpr const char* const INFO_BUTTON_TOOLTIP_MESSAGE{ "Show only messages with severity INFO or higher" };
	static constexpr const char* const WARNING_BUTTON_TOOLTIP_MESSAGE{ "Show only messages with severity WARNING or higher" };
	static constexpr const char* const ERROR_BUTTON_TOOLTIP_MESSAGE{ "Show only ERROR and FATAL messages" };
	static constexpr const char* const FATAL_BUTTON_TOOLTIP_MESSAGE{ "Show only FATAL messages" };

	static constexpr const char* const CLEAR_BUTTON_TEXT{ "Clear" };
	static constexpr const char* const CLEAR_BUTTON_TOOLTIP_TEXT{ "Clear the console" };

	GUI* const gui{ nullptr };

	Console(GUI* gui) noexcept;

	void Begin();
	void Render();
	void End();

private:
	float lineHeight{ 0 };
	ImVec2 buttonSize{ 0, 0 };

	LogSeverity selectedSeverity{ LogSeverity::TRACE };

	int oldCount{ 0 };

	void RenderTopPanel();
	void RenderFilterButtons();
	void RenderDummyArea();
	void RenderClearButton();

	void RenderMessageLog();
};
