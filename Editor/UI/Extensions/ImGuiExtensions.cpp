#include "ImGuiExtensions.hpp"

void DrawRowsBackground(int row_count, ImVec4 col_even, ImVec4 col_odd) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float item_spacing_y = ImGui::GetStyle().ItemSpacing.y;
	float item_offset_y = -item_spacing_y * 0.5f;
	float y0 = ImGui::GetCursorScreenPos().y + (float) (int) item_offset_y;
	float line_height = ImGui::GetTextLineHeight() + item_spacing_y;

	float x1 = ImGui::GetWindowPos().x;
	float x2 = x1 + ImGui::GetWindowSize().x;

	int row_display_start;
	int row_display_end;
	ImGui::CalcListClipping(row_count, line_height, &row_display_start, &row_display_end);
	for (int row_n = row_display_start; row_n < row_display_end; row_n++) {
		ImVec4& col = (row_n & 1) ? col_odd : col_even;
		if (col.w == 0) {
			continue;
		}
		float y1 = y0 + (line_height * row_n);
		float y2 = y1 + line_height;
		draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(col));
	}
}

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

float BeginTableColumnCenterText(std::string_view text) {
	float oldX = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth(ImGui::TableGetColumnIndex()) - ImGui::CalcTextSize(text.data()).x) / 2.0f);
	return oldX;
}
void EndTableColumnCenterText(float returnValueOfBegin) {
	ImGui::SetCursorPosX(returnValueOfBegin);
}

namespace rotation {
	static int start_index;
}
void ImRotateStart() {
	rotation::start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}
ImVec2 ImRotationCenter() {
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

	const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation::start_index; i < buf.Size; i++) {
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);
	}

	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}
void ImRotateEnd(float rad, ImVec2 center) {
	float s = sin(rad), c = cos(rad);
	ImVec2 cntr{ ImRotate(center, c, s) };
	center = { cntr.x - center.x, cntr.y - center.y };

	auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation::start_index; i < buf.Size; i++) {
		ImVec2 bufpos{ ImRotate(buf[i].pos, c, s) };
		buf[i].pos = { bufpos.x - center.x, bufpos.y - center.y };
	}
}