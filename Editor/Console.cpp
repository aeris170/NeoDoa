#include "Console.hpp"

#include "GUI.hpp"
#include "Icons.hpp"
#include "Colors.hpp"
#include "ImGuiExtensions.hpp"

Console::Console(GUI& gui) noexcept :
    gui(gui) {}

bool Console::Begin() {
    GUI& gui = this->gui;
    ImGui::PushID(GUI::CONSOLE_TITLE);
    std::string title(WindowIcons::CONSOLE_WINDOW_ICON);
    title.append(GUI::CONSOLE_TITLE);
    title.append(GUI::CONSOLE_ID);
    bool visible = ImGui::Begin(title.c_str());
    ImGui::SetWindowFontScale(0.9f);

    return visible;
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
    ImGui::PushStyleColor(ImGuiCol_Text, ConsoleColors::TRACE_COLOR);
    if (selectedSeverity == LogSeverity::TRACE) {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
        ImGui::PushFont(gui.GetFontBold());
        ImGui::Button(ConsoleIcons::TRACE_ICON, buttonSize);
        ImGui::PopFont();
        ImGui::PopStyleColor(2);
    } else if (ImGui::Button(ConsoleIcons::TRACE_ICON, buttonSize)) {
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
    ImGui::PushStyleColor(ImGuiCol_Text, ConsoleColors::INFO_COLOR);
    if (selectedSeverity == LogSeverity::INFO) {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
        ImGui::PushFont(gui.GetFontBold());
        ImGui::Button(ConsoleIcons::INFO_ICON, buttonSize);
        ImGui::PopFont();
        ImGui::PopStyleColor(2);
    } else if (ImGui::Button(ConsoleIcons::INFO_ICON, buttonSize)) {
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
    ImGui::PushStyleColor(ImGuiCol_Text, ConsoleColors::WARNING_COLOR);
    if (selectedSeverity == LogSeverity::WARNING) {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
        ImGui::PushFont(gui.GetFontBold());
        ImGui::Button(ConsoleIcons::WARNING_ICON, buttonSize);
        ImGui::PopFont();
        ImGui::PopStyleColor(2);
    } else if (ImGui::Button(ConsoleIcons::WARNING_ICON, buttonSize)) {
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
    ImGui::PushStyleColor(ImGuiCol_Text, ConsoleColors::ERROR_COLOR);
    if (selectedSeverity == LogSeverity::ERRO) {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
        ImGui::PushFont(gui.GetFontBold());
        ImGui::Button(ConsoleIcons::ERROR_ICON, buttonSize);
        ImGui::PopFont();
        ImGui::PopStyleColor(2);
    } else if (ImGui::Button(ConsoleIcons::ERROR_ICON, buttonSize)) {
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
    ImGui::PushStyleColor(ImGuiCol_Text, ConsoleColors::FATAL_COLOR);
    if (selectedSeverity == LogSeverity::FATAL) {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
        ImGui::PushFont(gui.GetFontBold());
        ImGui::Button(ConsoleIcons::FATAL_ICON, buttonSize);
        ImGui::PopFont();
        ImGui::PopStyleColor(2);
    } else if (ImGui::Button(ConsoleIcons::FATAL_ICON, buttonSize)) {
        selectedSeverity = LogSeverity::FATAL;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(FATAL_BUTTON_TOOLTIP_MESSAGE);
    }
    ImGui::PopStyleColor();
    ImGui::PopID();
#pragma endregion
}

void Console::RenderDummyArea() const {
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
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY | ImGuiTableFlags_PadOuterX,
        { ImGui::GetContentRegionAvail() }
    );
    if (!visible) return;

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    const auto& messages = Log::_messages;
    for (const auto& message : messages) {
        if (selectedSeverity <= message._severity) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            const char* icon;
            ImVec4 color;
            const char* tooltipMessage;

            switch (message._severity) {
            using enum LogSeverity;
            case TRACE:
                icon = ConsoleIcons::TRACE_ICON;
                color = ConsoleColors::TRACE_COLOR;
                tooltipMessage = TRACE_TOOLTIP_MESSAGE;
                break;
            case INFO:
                icon = ConsoleIcons::INFO_ICON;
                color = ConsoleColors::INFO_COLOR;
                tooltipMessage = INFO_TOOLTIP_MESSAGE;
                break;
            case WARNING:
                icon = ConsoleIcons::WARNING_ICON;
                color = ConsoleColors::WARNING_COLOR;
                tooltipMessage = WARNING_TOOLTIP_MESSAGE;
                break;
            case ERRO:
                icon = ConsoleIcons::ERROR_ICON;
                color = ConsoleColors::ERROR_COLOR;
                tooltipMessage = ERROR_TOOLTIP_MESSAGE;
                break;
            case FATAL:
                icon = ConsoleIcons::FATAL_ICON;
                color = ConsoleColors::FATAL_COLOR;
                tooltipMessage = FATAL_TOOLTIP_MESSAGE;
                break;
            case OPENGL:
                icon = ConsoleIcons::OPENGL_ICON;
                color = ConsoleColors::OPENGL_COLOR;
                tooltipMessage = OPENGL_TOOLTIP_MESSAGE;
                break;
            case VULKAN:
                icon = ConsoleIcons::VULKAN_ICON;
                color = ConsoleColors::VULKAN_COLOR;
                tooltipMessage = VULKAN_TOOLTIP_MESSAGE;
                break;
            case DIRECTX:
                icon = ConsoleIcons::DIRECTX_ICON;
                color = ConsoleColors::DIRECTX_COLOR;
                tooltipMessage = DIRECTX_TOOLTIP_MESSAGE;
                break;
            default:
                icon = "??";
                color = { 1, 0, 1, 1 };
                tooltipMessage = "this shouldn't be here";
                break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::PushFont(gui.GetFontBold());

            float r = BeginTableColumnCenterText(icon);
            ImGui::Text(icon);
            EndTableColumnCenterText(r);
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text(tooltipMessage);
                ImGui::EndTooltip();
            }
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