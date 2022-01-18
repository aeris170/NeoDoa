#include "ImGuiExtensions.hpp"

#include <imgui.h>
#include <imgui_internal.h>

bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    ImGuiID id = window->GetID("##Splitter");

    ImVec2 size(split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    ImVec2 itemSize(CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f));
    ImRect bb;
    bb.Min = ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y);
    bb.Max = ImVec2(bb.Min.x + itemSize.x, bb.Min.y + itemSize.y);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}