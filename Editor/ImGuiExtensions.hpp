#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <ImGuiRenderer.hpp>

void DrawRowsBackground(int row_count, ImVec4 col_even = WINDOW_BG, ImVec4 col_odd = WINDOW_BG_ALT);
bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);