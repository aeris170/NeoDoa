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

	bool rv{ false };

	auto& ctx = *ImGui::GetCurrentContext();
	if (ImGui::ColorButton(ss.str().c_str(), *reinterpret_cast<ImVec4*>(value.Data()), 0, { compFieldWidth , ImGui::GetFrameHeight() })) {
		ctx.ColorPickerRef = *reinterpret_cast<ImVec4*>(value.Data());
		ImGui::OpenPopup("picker");
		ImVec2 v;
		v.x = ctx.LastItemData.Rect.GetBL().x - 1;
		v.y = ctx.LastItemData.Rect.GetBL().y + ImGui::GetStyle().ItemSpacing.y;
		ImGui::SetNextWindowPos(v);
	}

	if (ImGui::BeginPopup("picker")) {
		//picker_active_window = ctx.CurrentWindow;
		ImGui::TextEx(ss.str().c_str());
		ImGui::Spacing();

		ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_DataTypeMask_ |
			ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ |
			ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha |
			ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayMask_ |
			ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;

		ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 12.0f);
		rv |= ImGui::ColorPicker4("##picker", value.Data(), 0, &ctx.ColorPickerRef.x);
		ImGui::EndPopup();
	}

	EndWidget();
	return rv;
}

bool ResolutionWidget(const std::string& label, Resolution& resolution) {
	glm::vec2 res{ resolution.w, resolution.h };
	FancyVectorWidgetSettings<display::XY> settingsFBO;
	settingsFBO.resetEnabled = false;
	settingsFBO.speed = 1;
	settingsFBO.min = 128;
	settingsFBO.max = 128*128;
	settingsFBO.fmt = "%g";
	settingsFBO.displayLabelOverride[0] = "W";
	settingsFBO.displayLabelOverride[1] = "H";
	settingsFBO.displayLabelColorOverride[0] = Color(0.45, 0.63, 0.82);
	settingsFBO.displayLabelColorOverride[1] = Color(0.66, 0.49, 0.65);

	bool rv = FancyVector2Widget(label, res, settingsFBO);
	resolution = { static_cast<int>(res.x), static_cast<int>(res.y) };

	return rv;
}

bool OrthoCameraWidget(OrthoCamera& cameraData) {
	glm::vec1 top{ cameraData._top };
	glm::vec2 leftRight{ cameraData._left, cameraData._right };
	glm::vec1 bottom{ cameraData._bottom };

	FancyVectorWidgetSettings<display::X> settingsOrthoTop;
	settingsOrthoTop.resetTo = 1;
	settingsOrthoTop.displayLabelOverride[0] = "T";
	settingsOrthoTop.displayLabelColorOverride[0] = Color(0.10, 0.53, 0.26);
	settingsOrthoTop.displayLabelHoverColorOverride[0] = settingsOrthoTop.displayLabelColorOverride[0].Lighten(0.45f);
	settingsOrthoTop.componentFieldWidth /= 2;

	FancyVectorWidgetSettings<display::XY> settingsOrthoLeftRight;
	settingsOrthoLeftRight.resetAllToSame = false;
	settingsOrthoLeftRight.resetTos[0] = -1;
	settingsOrthoLeftRight.resetTos[1] = 1;
	settingsOrthoLeftRight.displayLabelOverride[0] = "L";
	settingsOrthoLeftRight.displayLabelOverride[1] = "R";
	settingsOrthoLeftRight.displayLabelColorOverride[0] = Color(0.10, 0.53, 0.26);
	settingsOrthoLeftRight.displayLabelColorOverride[1] = Color(0.10, 0.53, 0.26);
	settingsOrthoLeftRight.displayLabelHoverColorOverride[0] = settingsOrthoLeftRight.displayLabelColorOverride[0].Lighten(0.45f);
	settingsOrthoLeftRight.displayLabelHoverColorOverride[1] = settingsOrthoLeftRight.displayLabelColorOverride[1].Lighten(0.45f);

	FancyVectorWidgetSettings<display::X> settingsOrthoBottom;
	settingsOrthoBottom.resetTo = -1;
	settingsOrthoBottom.displayLabelOverride[0] = "B";
	settingsOrthoBottom.displayLabelColorOverride[0] = Color(0.10, 0.53, 0.26 );
	settingsOrthoBottom.displayLabelHoverColorOverride[0] = settingsOrthoBottom.displayLabelColorOverride[0].Lighten(0.45f);
	settingsOrthoBottom.componentFieldWidth /= 2;

	bool rv{ false };
	rv = FancyVector1Widget("Top", top, settingsOrthoTop);
	rv = rv | FancyVector2Widget("Left & Right", leftRight, settingsOrthoLeftRight); // no double pipe because of short circuit shadowing imgui call
	rv = rv | FancyVector1Widget("Bottom", bottom, settingsOrthoBottom);

	cameraData._top = top.x;
	cameraData._left = leftRight.x;
	cameraData._right = leftRight.y;
	cameraData._bottom = bottom.x;

	return rv;
}

bool PerspectiveCameraWidget(PerspectiveCamera& cameraData) {
	glm::vec1 fov{ cameraData._fov };
	glm::vec1 aspect{ cameraData._aspect };

	FancyVectorWidgetSettings<display::X> fovSettings;
	auto a = fovSettings.displayLabelColorOverride[0].Lighten(0.5);
	fovSettings.resetTo = 110;
	fovSettings.min = 30;
	fovSettings.max = 150;
	fovSettings.displayLabelOverride[0] = "F";
	fovSettings.displayLabelColorOverride[0] = Color(0.23, 0.23, 0.13);
	fovSettings.displayLabelHoverColorOverride[0] = fovSettings.displayLabelColorOverride[0].Lighten(0.35f);

	FancyVectorWidgetSettings<display::X> aspectSettings;
	aspectSettings.disabled = true;
	aspectSettings.displayLabelOverride[0] = "A";
	aspectSettings.displayLabelColorOverride[0] = Color(0.211, 0.121, 0.439);

	bool rv{ false };
	rv = FancyVector1Widget("Field Of View", fov, fovSettings);
	rv = rv | FancyVector1Widget("Aspect Ratio", aspect, aspectSettings); // no double pipe because of short circuit shadowing imgui call

	cameraData._fov = fov.x;

	return rv;
}

void Space() {
	ImGui::NewLine();
}

void Header(const std::string& label) {
	ImGui::PushID(label.c_str());

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];
	ImGui::PushFont(boldFont);

	ImGui::Text(label.c_str());
	ImGui::Separator();

	ImGui::PopFont();
	ImGui::PopID();
}

void Separator() {
	ImGui::NewLine();
	ImGui::Separator();
}
