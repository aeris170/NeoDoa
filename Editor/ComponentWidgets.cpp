#include "ComponentWidgets.hpp"

#include <imgui.h>
#include <imgui_internal.h>

bool FancyVector1Widget(const std::string& label, glm::vec1& vec, FancyVectorWidgetSettings<display::X> settings) {
	return FancyVectorWidget(label, &vec.x, settings);
}

bool FancyVector2Widget(const std::string& label, glm::vec2& vec, FancyVectorWidgetSettings<display::XY> settings) {
	return FancyVectorWidget(label, &vec.x, settings);
}

bool FancyVector3Widget(const std::string& label, glm::vec3& vec, FancyVectorWidgetSettings<display::XYZ> settings) {
	return FancyVectorWidget(label, &vec.x, settings);
}

bool FancyVector4Widget(const std::string& label, glm::vec4& vec, FancyVectorWidgetSettings<display::XYZW> settings) {
	return FancyVectorWidget(label, &vec.x, settings);
}

void detail::BeginWidget(const std::string& label) {
	float w = ImGui::GetContentRegionAvail().x;

	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0 });
	ImGui::SetColumnWidth(0, w - compFieldWidth);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f + 3);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);

	ImGui::PushItemWidth(compFieldWidth);
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];
	ImGui::PushFont(boldFont);
}

void detail::EndWidget() {
	ImGui::PopFont();
	ImGui::PopStyleVar(2);
	ImGui::Columns(1);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

bool EnumWidget(const std::string& label, int& value, const std::vector<EnumValue>& values) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;

	int selected = -1;
	std::vector<const char*> items;
	for (int i = 0; i < values.size(); i++) {
		auto& element = values[i];
		if (element.value == value) {
			selected = i;
		}
		items.push_back(element.prettyName.c_str());
	}

	bool rv{ false };
	if(ImGui::BeginCombo(ss.str().c_str(), values[selected].prettyName.c_str())) {
		for (int i = 0; i < values.size(); i++) {
			bool is_selected = (value == values[i].value); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(values[i].prettyName.c_str(), is_selected)) {
				rv = true;
				value = values[i].value;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}
	EndWidget();

	return rv;
}

bool EntityWidget(const std::string& label, Entity& value) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	int val = EntityTo<int>(value);
	bool rv = ImGui::DragInt(ss.str().c_str(), &val);
	value = static_cast<Entity>(val);
	EndWidget();
	return rv;
}

void UneditableEntityWidget(const std::string& label, const Entity value) {
	BeginWidget(label);
	int val = EntityTo<int>(value);
	ImGui::Text(std::to_string(val).c_str());
	EndWidget();
}

bool IntWidget(const std::string& label, int& value) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	bool rv = ImGui::DragInt(ss.str().c_str(), &value);
	EndWidget();
	return rv;
}

bool FloatWidget(const std::string& label, float& value, float step, float min, float max) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	bool rv = ImGui::DragFloat(ss.str().c_str(), &value, step, min, max);
	EndWidget();
	return rv;
}

bool DoubleWidget(const std::string& label, double& value) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_Double, (void*)&value, 0.1f);
	EndWidget();
	return rv;
}

bool BoolWidget(const std::string& label, bool& value) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	bool rv = ImGui::Checkbox(ss.str().c_str(), &value);
	EndWidget();
	return rv;
}

bool StringWidget(const std::string& label, std::string& value) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	char buf[128]{ 0 };
	strcpy_s(buf, value.c_str());
	bool rv{ false };
	if (ImGui::InputText(ss.str().c_str(), buf, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
		rv = true;
		value = std::string(buf);
	};
	EndWidget();
	return rv;
}

void UneditableStringWidget(const std::string& label, const std::string& value) {
	BeginWidget(label);
	ImGui::Text(value.c_str());
	EndWidget();
}

bool ColorWidget(const std::string& label, Color& value) {
	BeginWidget(label);
	std::stringstream ss;
	ss << "##" << label;
	bool rv = ImGui::ColorEdit4(ss.str().c_str(), value.Data());
	EndWidget();
	return rv;
}
