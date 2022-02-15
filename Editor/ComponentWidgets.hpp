#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

#include "NeoDoa.hpp"

inline int X = 0b0001;
inline int Y = 0b0010;
inline int Z = 0b0100;
inline int W = 0b1000;

inline constexpr int compFieldWidth = 276; // must be divisible by both 3 and 4 (and 1 and 2, but you know... MaThS...) (x,y,z,w)

enum class display {
	X = 1, XY = 2, XYZ = 3, XYZW = 4
};

template<display dsp>
struct FancyVectorWidgetSettings {
	bool resetEnabled{ true };
	bool resetAllToSame{ true };
	float resetTo{ 0.0f };
	std::array<float, static_cast<size_t>(dsp)> resetTos{};
	display display{ dsp };
	float speed{ 0.1f };
	float min{ 0.0f };
	float max{ 0.0f };
	const char* fmt{ "%.3f" };
	int componentFieldWidth{ compFieldWidth };
	std::array<const char*, static_cast<size_t>(dsp)> displayLabelOverride{};
	std::array<Color, static_cast<size_t>(dsp)> displayLabelColorOverride{};
	std::array<Color, static_cast<size_t>(dsp)> displayLabelHoverColorOverride{};
	std::array<const char*, static_cast<size_t>(dsp)> displayLabelID{};

	constexpr FancyVectorWidgetSettings() {
		displayLabelOverride[0] = "X";
		displayLabelColorOverride[0] = Color(0.80f, 0.10f, 0.15f);
		displayLabelHoverColorOverride[0] = Color(0.94f, 0.51f, 0.55f);
		displayLabelID[0] = "###X";

		if constexpr (dsp == display::XY) {
			displayLabelOverride[1] = "Y";
			displayLabelColorOverride[1] = Color(0.20f, 0.70f, 0.20f);
			displayLabelHoverColorOverride[1] = Color(0.57f, 0.87f, 0.57f);
			displayLabelID[1] = "###Y";
		}

		if constexpr (dsp == display::XYZ) {
			displayLabelOverride[1] = "Y";
			displayLabelColorOverride[1] = Color(0.20f, 0.70f, 0.20f);
			displayLabelHoverColorOverride[1] = Color(0.57f, 0.87f, 0.57f);
			displayLabelID[1] = "###Y";

			displayLabelOverride[2] = "Z";
			displayLabelColorOverride[2] = Color(0.10f, 0.25f, 0.80f);
			displayLabelHoverColorOverride[2] = Color(0.51f, 0.60f, 0.94f);
			displayLabelID[2] = "###Z";
		}

		if constexpr (dsp == display::XYZW) {
			displayLabelOverride[1] = "Y";
			displayLabelColorOverride[1] = Color(0.20f, 0.70f, 0.20f);
			displayLabelHoverColorOverride[1] = Color(0.57f, 0.87f, 0.57f);
			displayLabelID[1] = "###Y";

			displayLabelOverride[2] = "Z";
			displayLabelColorOverride[2] = Color(0.10f, 0.25f, 0.80f);
			displayLabelHoverColorOverride[2] = Color(0.51f, 0.60f, 0.94f);
			displayLabelID[2] = "###Z";

			displayLabelOverride[3] = "W";
			displayLabelColorOverride[3] = Color(0.61f, 0.29f, 0.85f);
			displayLabelHoverColorOverride[3] = Color(0.80f, 0.64f, 0.93f);
			displayLabelID[3] = "###W";
		}
	}
};
inline constexpr FancyVectorWidgetSettings<display::X> defaultFancyVectorSettingsX;
inline constexpr FancyVectorWidgetSettings<display::XY> defaultFancyVectorSettingsXY;
inline constexpr FancyVectorWidgetSettings<display::XYZ> defaultFancyVectorSettingsXYZ;
inline constexpr FancyVectorWidgetSettings<display::XYZW> defaultFancyVectorSettingsXYZW;

bool FancyVector1Widget(const std::string& label, glm::vec1& vec, FancyVectorWidgetSettings<display::X> settings = defaultFancyVectorSettingsX);
bool FancyVector2Widget(const std::string& label, glm::vec2& vec, FancyVectorWidgetSettings<display::XY> settings = defaultFancyVectorSettingsXY);
bool FancyVector3Widget(const std::string& label, glm::vec3& vec, FancyVectorWidgetSettings<display::XYZ> settings = defaultFancyVectorSettingsXYZ);
bool FancyVector4Widget(const std::string& label, glm::vec4& vec, FancyVectorWidgetSettings<display::XYZW> settings = defaultFancyVectorSettingsXYZW);

template<display dsp>
bool FancyVectorWidget(const std::string& label, float* vec, FancyVectorWidgetSettings<dsp> settings = defaultFancyVectorSettingsXYZW) {
	bool rv{ false };

	float w = ImGui::GetContentRegionAvail().x;

	ImGui::PushItemWidth(settings.componentFieldWidth);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0 });
	ImGui::SetColumnWidth(0, w - compFieldWidth);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (compFieldWidth - settings.componentFieldWidth) / 2);

	float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	auto RenderFancyVectorComponentPiece = [&rv, &boldFont, &buttonSize, &vec](FancyVectorWidgetSettings<dsp>& settings, size_t idx) {
		auto btnColor = settings.displayLabelColorOverride[idx];
		auto btnHoverColor = settings.displayLabelHoverColorOverride[idx];
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ btnColor.r, btnColor.g, btnColor.b, btnColor.a });
		if (settings.resetEnabled) {
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ btnHoverColor.r, btnHoverColor.g, btnHoverColor.b, btnHoverColor.a });
		} else {
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ btnColor.r, btnColor.g, btnColor.b, btnColor.a });
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ btnColor.r, btnColor.g, btnColor.b, btnColor.a });
		ImGui::PushFont(boldFont);
		if (ImGui::Button(settings.displayLabelOverride[idx], buttonSize)) {
			if (settings.resetEnabled) {
				rv = true;
				vec[idx] = settings.resetTo;
			}
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(settings.componentFieldWidth / static_cast<int>(dsp) - buttonSize.x);

		if (ImGui::DragFloat(std::string("##").append(settings.displayLabelOverride[idx]).c_str(), &(vec[idx]), settings.speed, settings.min, settings.max, settings.fmt)) {
			rv = true;
		}
		ImGui::PopItemWidth();
	};

	RenderFancyVectorComponentPiece(settings, 0);
	if constexpr (dsp == display::XY) {
		ImGui::SameLine();
		RenderFancyVectorComponentPiece(settings, 1);
	}
	if constexpr (dsp == display::XYZ) {
		ImGui::SameLine();
		RenderFancyVectorComponentPiece(settings, 1);
		ImGui::SameLine();
		RenderFancyVectorComponentPiece(settings, 2);
	}
	if constexpr (dsp == display::XYZW) {
		ImGui::SameLine();
		RenderFancyVectorComponentPiece(settings, 1);
		ImGui::SameLine();
		RenderFancyVectorComponentPiece(settings, 2);
		ImGui::SameLine();
		RenderFancyVectorComponentPiece(settings, 3);
	}

	ImGui::PopStyleVar(2);

	ImGui::Columns(1);

	ImGui::PopID();

	return rv;
}

namespace detail {
	void BeginWidget(const std::string& label);
	void EndWidget();
}

bool EnumWidget(const std::string& label, int& value, const std::vector<EnumValue>& values);

bool EntityWidget(const std::string& label, Entity& value);

void UneditableEntityWidget(const std::string& label, const Entity value);

bool IntWidget(const std::string& label, int& value);

bool FloatWidget(const std::string& label, float& value, float step = 0.1f, float min = 0.0f, float max = 0.0f);

bool DoubleWidget(const std::string& label, double& value);

bool BoolWidget(const std::string& label, bool& value);

bool StringWidget(const std::string& label, std::string& value);

void UneditableStringWidget(const std::string& label, const std::string& value);

bool ColorWidget(const std::string& label, Color& value);

using namespace detail;

template<typename T>
requires requires (T x) { std::to_string(x); }
void UneditableArrayWidget(const std::string& label, const std::vector<T>& array) {
	BeginWidget(label);
	for (int i = 0; i < array.size(); i++) {
		ImGui::Text(std::to_string(array[i]).c_str());
	}
	EndWidget();
}