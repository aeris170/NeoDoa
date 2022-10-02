#include "Console.hpp"

#include <IconsFontAwesome5Pro.h>

#include "GUI.hpp"
#include "Icons.hpp"

const ImVec4 Console::TRACE_COLOR{ 0.7f, 0.7f, 0.7f, 1.0f };
const ImVec4 Console::INFO_COLOR{ 0.7f, 1.0f, 0.7f, 1.0f };
const ImVec4 Console::WARNING_COLOR{ 1.0f, 0.5f, 0.1f, 1.0f };
const ImVec4 Console::ERROR_COLOR{ 1.0f, 0.6f, 0.6f, 1.0f };
const ImVec4 Console::FATAL_COLOR{ 1.0f, 0.4f, 0.4f, 1.0f };
const ImVec4 Console::OPENGL_COLOR{ 0.32f, 0.51f, 0.62f, 1.0f };
const ImVec4 Console::VULKAN_COLOR{ 0.62f, 0.11f, 0.13f, 1.0f };
const ImVec4 Console::DIRECTX_COLOR{ 0.48f, 0.71f, 0.0f, 1.0f };

Console::Console(GUI& gui) noexcept :
	gui(gui) {}

void Console::Begin() {
	GUI& gui = this->gui;
	ImGui::PushID(gui.CONSOLE_TITLE);
	std::string title(WindowIcons::CONSOLE_WINDOW_ICON);
	title.append(gui.CONSOLE_TITLE);
	title.append(gui.CONSOLE_ID);
	ImGui::Begin(title.c_str());
	ImGui::SetWindowFontScale(0.9f);
}

void Console::Render() {
	RenderTopPanel();
	RenderMessageLog();
}

void Console::End() {
	ImGui::SetWindowFontScale(1);
	ImGui::End();
	ImGui::PopID();
}

void Console::RenderTopPanel() {
	GUI& gui = this->gui;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

	lineHeight = gui.GetFont()->FontSize * 0.9f + ImGui::GetStyle().FramePadding.y * 2.0f;
	buttonSize = { lineHeight + 6.0f, lineHeight };

	ImGui::PushFont(gui.GetFont());
	RenderFilterButtons();
	ImGui::PopFont();
	ImGui::SameLine();
	RenderDummyArea();
	ImGui::SameLine();
	RenderClearButton();

	ImGui::PopStyleVar(2);
}

void Console::RenderFilterButtons() {
	GUI& gui = this->gui;
#pragma region Trace
	ImGui::PushID("TRACE");
	ImGui::PushStyleColor(ImGuiCol_Text, TRACE_COLOR);
	if (selectedSeverity == LogSeverity::TRACE) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushFont(gui.GetFontBold());
		ImGui::Button(ICON_FA_INFO, buttonSize);
		ImGui::PopFont();
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_INFO, buttonSize)) {
		selectedSeverity = LogSeverity::TRACE;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(TRACE_BUTTON_TOOLTIP_MESSAGE);
	}
	ImGui::PopStyleColor();
	ImGui::PopID();
#pragma endregion

	ImGui::SameLine();

#pragma region Info
	ImGui::PushID("INFO");
	ImGui::PushStyleColor(ImGuiCol_Text, INFO_COLOR);
	if (selectedSeverity == LogSeverity::INFO) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushFont(gui.GetFontBold());
		ImGui::Button(ICON_FA_INFO, buttonSize);
		ImGui::PopFont();
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_INFO, buttonSize)) {
		selectedSeverity = LogSeverity::INFO;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(INFO_BUTTON_TOOLTIP_MESSAGE);
	}
	ImGui::PopStyleColor();
	ImGui::PopID();
#pragma endregion

	ImGui::SameLine();

#pragma region Warning
	ImGui::PushID("WARNING");
	ImGui::PushStyleColor(ImGuiCol_Text, WARNING_COLOR);
	if (selectedSeverity == LogSeverity::WARNING) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushFont(gui.GetFontBold());
		ImGui::Button(ICON_FA_EXCLAMATION_CIRCLE, buttonSize);
		ImGui::PopFont();
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_EXCLAMATION_CIRCLE, buttonSize)) {
		selectedSeverity = LogSeverity::WARNING;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(WARNING_BUTTON_TOOLTIP_MESSAGE);
	}
	ImGui::PopStyleColor();
	ImGui::PopID();
#pragma endregion

	ImGui::SameLine();

#pragma region Error
	ImGui::PushID("ERROR");
	ImGui::PushStyleColor(ImGuiCol_Text, ERROR_COLOR);
	if (selectedSeverity == LogSeverity::ERRO) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushFont(gui.GetFontBold());
		ImGui::Button(ICON_FA_EXCLAMATION_TRIANGLE, buttonSize);
		ImGui::PopFont();
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_EXCLAMATION_TRIANGLE, buttonSize)) {
		selectedSeverity = LogSeverity::ERRO;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(ERROR_BUTTON_TOOLTIP_MESSAGE);
	}
	ImGui::PopStyleColor();
	ImGui::PopID();
#pragma endregion

	ImGui::SameLine();

#pragma region Fatal
	ImGui::PushID("FATAL");
	ImGui::PushStyleColor(ImGuiCol_Text, FATAL_COLOR);
	if (selectedSeverity == LogSeverity::FATAL) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushFont(gui.GetFontBold());
		ImGui::Button(ICON_FA_EXCLAMATION_TRIANGLE, buttonSize);
		ImGui::PopFont();
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_EXCLAMATION_TRIANGLE, buttonSize)) {
		selectedSeverity = LogSeverity::FATAL;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(FATAL_BUTTON_TOOLTIP_MESSAGE);
	}
	ImGui::PopStyleColor();
	ImGui::PopID();
#pragma endregion
}

void Console::RenderDummyArea() {
	ImGui::Dummy({ ImGui::GetContentRegionAvail().x - 60, buttonSize.y });
}

void Console::RenderClearButton() {
	GUI& gui = this->gui;
	ImGui::PushFont(gui.GetFontBold());
	if (ImGui::Button(CLEAR_BUTTON_TEXT, { 60, buttonSize.y })) {
		Log::Clear();
		oldCount = 0;
	}
	ImGui::PopFont();
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(CLEAR_BUTTON_TOOLTIP_TEXT);
	}
}

void Console::RenderMessageLog() {
	GUI& gui = this->gui;
	bool visible = ImGui::BeginTable(
		"log",
		2,
		ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY,
		{ ImGui::GetContentRegionAvail() }
	);
	if (!visible) return;

	ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

	auto& messages = Log::_messages;
	for (auto& message : messages) {
		if (selectedSeverity <= message._severity) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			const char* icon;
			ImVec4 color;
			const char* tooltipMessage;

			float oldX = ImGui::GetCursorPosX();
			switch (message._severity) {
			case LogSeverity::TRACE:
				icon = ConsoleIcons::TRACE_ICON;
				color = TRACE_COLOR;
				tooltipMessage = TRACE_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 14);
				break;
			case LogSeverity::INFO:
				icon = ConsoleIcons::INFO_ICON;
				color = INFO_COLOR;
				tooltipMessage = INFO_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 14);
				break;
			case LogSeverity::WARNING:
				icon = ConsoleIcons::WARNING_ICON;
				color = WARNING_COLOR;
				tooltipMessage = WARNING_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 9);
				break;
			case LogSeverity::ERRO:
				icon = ConsoleIcons::ERROR_ICON;
				color = ERROR_COLOR;
				tooltipMessage = ERROR_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 8);
				break;
			case LogSeverity::FATAL:
				icon = ConsoleIcons::FATAL_ICON;
				color = FATAL_COLOR;
				tooltipMessage = FATAL_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 8);
				break;
			case LogSeverity::OPENGL:
				icon = ConsoleIcons::OPENGL_ICON;
				color = OPENGL_COLOR;
				tooltipMessage = OPENGL_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 9);
				break;
			case LogSeverity::VULKAN:
				icon = ConsoleIcons::VULKAN_ICON;
				color = VULKAN_COLOR;
				tooltipMessage = VULKAN_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 9);
				break;
			case LogSeverity::DIRECTX:
				icon = ConsoleIcons::DIRECTX_ICON;
				color = DIRECTX_COLOR;
				tooltipMessage = DIRECTX_TOOLTIP_MESSAGE;
				ImGui::SetCursorPosX(oldX + 9);
				break;
			default:
				icon = "??";
				color = { 1, 0, 1, 1 };
				tooltipMessage = "this shouldn't be here";
				break;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);

			ImGui::PushFont(gui.GetFontBold());
			ImGui::Text(icon);
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text(tooltipMessage);
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPosX(oldX);
			ImGui::PopFont();

			ImGui::TableSetColumnIndex(1);
			ImGui::PushFont(gui.GetFontBold());
			ImGui::Text(message._message.c_str());
			ImGui::PopFont();

			ImGui::PopStyleColor();
		}
	}
	ImGui::PopStyleVar();

	if (oldCount < messages.size()) {
		ImGui::SetScrollY(ImGui::GetScrollMaxY() + 100);
		oldCount = static_cast<int>(messages.size());
	}
	ImGui::EndTable();
}