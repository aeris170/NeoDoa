#pragma once

#include <format>
#include <string_view>

#include <imgui.h>
#include <imgui_internal.h>

#include <Engine/ImGuiRenderer.hpp>

void DrawRowsBackground(int row_count, ImVec4 col_even = WINDOW_BG, ImVec4 col_odd = WINDOW_BG_ALT);
bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

[[nodiscard("The return value of BeginTableColumnCenterText is required for EndTableColumnCenterText")]]
float BeginTableColumnCenterText(std::string_view text);
void EndTableColumnCenterText(float returnValueOfBegin);

void ImRotateStart();
ImVec2 ImRotationCenter();
void ImRotateEnd(float rad = 0, ImVec2 center = ImRotationCenter());

void FocusNextItem();

template<class ...Args>
void ImGuiFormattedText(std::format_string<Args...> format, Args&&... args) {
    ImGui::TextUnformatted(std::format(format, std::forward<Args>(args)...).c_str());
}