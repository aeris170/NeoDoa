#include "ComponentWidgets.hpp"

#include <imgui.h>
#include <imgui_internal.h>

bool FancyVectorWidget(const std::string& label, glm::vec2& vec, float reset, int flags) {
	return FancyVectorWidget(label, &vec.x, reset, flags);
}

bool FancyVectorWidget(const std::string& label, glm::vec3& vec, float reset, int flags) {
	return FancyVectorWidget(label, &vec.x, reset, flags);
}

bool FancyVectorWidget(const std::string& label, glm::vec4& vec, float reset, int flags) {
	return FancyVectorWidget(label, &vec.x, reset, flags);
}

// Courtesy of Yan Chernikov, a.k.a The Cherno Project
bool FancyVectorWidget(const std::string& label, float* values, float reset, int flags) {
	bool rv{ false };
	float divisor = 0;
	if (flags & X) divisor++;
	if (flags & Y) divisor++;
	if (flags & Z) divisor++;
	if (flags & W) divisor++;

	float w = ImGui::GetContentRegionAvail().x;

	ImGui::PushItemWidth(compFieldWidth);

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

	float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	if(flags & X) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.94f, 0.51f, 0.55f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		ImVec2 hack = { buttonSize.x - 1, buttonSize.y }; // the color red likes to appear bigger than it is...
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 1);
		if (ImGui::Button("X", hack)) {
			rv = true;
			values[0] = reset;
		}
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(compFieldWidth / divisor -  buttonSize.x);
		rv = rv || ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		if (flags & Y) {
			ImGui::SameLine();
		}
	}

	if(flags & Y) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.57f, 0.87f, 0.57f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			rv = true;
			values[1] = reset;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(compFieldWidth / divisor - buttonSize.x);
		rv = rv || ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		if (flags & Z) {
			ImGui::SameLine();
		}
	}

	if(flags & Z) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.51f, 0.6f, 0.94f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			rv = true;
			values[2] = reset;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(compFieldWidth / divisor - buttonSize.x);
		rv = rv || ImGui::DragFloat("##Z", &values[2], 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		if(flags & W) {
			ImGui::SameLine();
		}
	}

	if (flags & W) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.61f, 0.29f, 0.85f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.80f, 0.64f, 0.93f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.61f, 0.29f, 0.85f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize)) {
			rv = true;
			values[3] = reset;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(compFieldWidth / divisor - buttonSize.x);
		rv = rv || ImGui::DragFloat("##W", &values[3], 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
	}
	ImGui::PopStyleVar(2);

	ImGui::Columns(1);

	ImGui::PopID();

	return rv;
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
