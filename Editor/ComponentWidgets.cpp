#include <Editor/ComponentWidgets.hpp>

#include <imgui.h>
#include <imgui_internal.h>

bool FancyVector1Widget(const std::string& label, glm::vec1& vec, FancyVectorWidgetSettings<Display::X> settings) {
    return FancyVectorWidget(label, &vec.x, settings);
}
bool FancyVector2Widget(const std::string& label, glm::vec2& vec, FancyVectorWidgetSettings<Display::XY> settings) {
    return FancyVectorWidget(label, &vec.x, settings);
}
bool FancyVector3Widget(const std::string& label, glm::vec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings) {
    return FancyVectorWidget(label, &vec.x, settings);
}
bool FancyVector4Widget(const std::string& label, glm::vec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings) {
    return FancyVectorWidget(label, &vec.x, settings);
}

bool FancyVectori1Widget(const std::string& label, glm::ivec1& vec, FancyVectorWidgetSettings<Display::X> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.fmt = "%.0f";

    std::array<float, 1> values = {
        static_cast<float>(vec.x),
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<int>(std::round(values[0]));

    return rv;
}
bool FancyVectori2Widget(const std::string& label, glm::ivec2& vec, FancyVectorWidgetSettings<Display::XY> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.fmt = "%.0f";

    std::array<float, 2> values = {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y)
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<int>(std::round(values[0]));
    vec.y = static_cast<int>(std::round(values[1]));

    return rv;
}
bool FancyVectori3Widget(const std::string& label, glm::ivec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.fmt = "%.0f";

    std::array<float, 3> values = {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y),
        static_cast<float>(vec.z)
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<int>(std::round(values[0]));
    vec.y = static_cast<int>(std::round(values[1]));
    vec.z = static_cast<int>(std::round(values[2]));

    return rv;
}
bool FancyVectori4Widget(const std::string& label, glm::ivec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.fmt = "%.0f";

    std::array<float, 4> values = {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y),
        static_cast<float>(vec.z),
        static_cast<float>(vec.w)
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<int>(std::round(values[0]));
    vec.y = static_cast<int>(std::round(values[1]));
    vec.z = static_cast<int>(std::round(values[2]));
    vec.w = static_cast<int>(std::round(values[3]));

    return rv;
}

bool FancyVectorui1Widget(const std::string& label, glm::uvec1& vec, FancyVectorWidgetSettings<Display::X> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.min = 0.0f;
    settings.max = static_cast<float>(std::numeric_limits<unsigned>::max());
    settings.fmt = "%.0f";

    float value = static_cast<float>(vec.x);
    bool rv = FancyVectorWidget(label, &value, settings);
    vec.x = static_cast<unsigned>(std::round(value));

    return rv;
}
bool FancyVectorui2Widget(const std::string& label, glm::uvec2& vec, FancyVectorWidgetSettings<Display::XY> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.min = 0.0f;
    settings.max = static_cast<float>(std::numeric_limits<unsigned>::max());
    settings.fmt = "%.0f";

    std::array<float, 2> values = {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y)
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<unsigned>(std::round(values[0]));
    vec.y = static_cast<unsigned>(std::round(values[1]));

    return rv;
}
bool FancyVectorui3Widget(const std::string& label, glm::uvec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.min = 0.0f;
    settings.max = static_cast<float>(std::numeric_limits<unsigned>::max());
    settings.fmt = "%.0f";

    std::array<float, 3> values = {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y),
        static_cast<float>(vec.z)
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<unsigned>(std::round(values[0]));
    vec.y = static_cast<unsigned>(std::round(values[1]));
    vec.z = static_cast<unsigned>(std::round(values[2]));

    return rv;
}
bool FancyVectorui4Widget(const std::string& label, glm::uvec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings) {
    settings.speed = std::clamp(std::round(settings.speed), 1.0f, static_cast<float>(std::numeric_limits<int>::max()));
    settings.min = 0.0f;
    settings.max = static_cast<float>(std::numeric_limits<unsigned>::max());
    settings.fmt = "%.0f";

    std::array<float, 4> values = {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y),
        static_cast<float>(vec.z),
        static_cast<float>(vec.w)
    };
    bool rv = FancyVectorWidget(label, values.data(), settings);
    vec.x = static_cast<unsigned>(std::round(values[0]));
    vec.y = static_cast<unsigned>(std::round(values[1]));
    vec.z = static_cast<unsigned>(std::round(values[2]));
    vec.w = static_cast<unsigned>(std::round(values[3]));

    return rv;
}

bool FancyVectorb1Widget(const std::string& label, glm::ivec1& vec, FancyVectorWidgetSettings<Display::X> settings) {
    settings.min = 0.0f;
    settings.max = 1.0f;
    settings.displayLabelOverride[0] = "B";
    settings.displayLabelColorOverride[0] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelHoverColorOverride[0] = Color(0.3f, 0.3f, 0.3f);

    return FancyVectori1Widget(label, vec, settings);
}
bool FancyVectorb2Widget(const std::string& label, glm::ivec2& vec, FancyVectorWidgetSettings<Display::XY> settings) {
    settings.min = 0.0f;
    settings.max = 1.0f;
    settings.displayLabelOverride[0] = "B";
    settings.displayLabelOverride[1] = "B";
    settings.displayLabelColorOverride[0] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelColorOverride[1] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelHoverColorOverride[0] = Color(0.3f, 0.3f, 0.3f);
    settings.displayLabelHoverColorOverride[1] = Color(0.3f, 0.3f, 0.3f);

    return FancyVectori2Widget(label, vec, settings);
}
bool FancyVectorb3Widget(const std::string& label, glm::ivec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings) {
    settings.min = 0.0f;
    settings.max = 1.0f;
    settings.displayLabelOverride[0] = "B";
    settings.displayLabelOverride[1] = "B";
    settings.displayLabelOverride[2] = "B";
    settings.displayLabelColorOverride[0] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelColorOverride[1] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelColorOverride[2] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelHoverColorOverride[0] = Color(0.3f, 0.3f, 0.3f);
    settings.displayLabelHoverColorOverride[1] = Color(0.3f, 0.3f, 0.3f);
    settings.displayLabelHoverColorOverride[2] = Color(0.3f, 0.3f, 0.3f);

    return FancyVectori3Widget(label, vec, settings);
}
bool FancyVectorb4Widget(const std::string& label, glm::ivec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings) {
    settings.min = 0.0f;
    settings.max = 1.0f;
    settings.displayLabelOverride[0] = "B";
    settings.displayLabelOverride[1] = "B";
    settings.displayLabelOverride[2] = "B";
    settings.displayLabelOverride[3] = "B";
    settings.displayLabelColorOverride[0] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelColorOverride[1] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelColorOverride[2] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelColorOverride[3] = Color(0.2f, 0.2f, 0.2f);
    settings.displayLabelHoverColorOverride[0] = Color(0.3f, 0.3f, 0.3f);
    settings.displayLabelHoverColorOverride[1] = Color(0.3f, 0.3f, 0.3f);
    settings.displayLabelHoverColorOverride[2] = Color(0.3f, 0.3f, 0.3f);
    settings.displayLabelHoverColorOverride[3] = Color(0.3f, 0.3f, 0.3f);

    return FancyVectori4Widget(label, vec, settings);
}

void detail::BeginWidget(const std::string& label) {
    float w = ImGui::GetContentRegionAvail().x;

    ImGui::PushID(label.c_str());
    ImGui::Columns(2, nullptr, false);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0 });
    ImGui::SetColumnWidth(0, w - compFieldWidth);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f + 3);
    ImGui::TextUnformatted(label.c_str());
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

    size_t selected{ std::numeric_limits<size_t>::max() };
    for (size_t i = 0; i < values.size(); i++) {
        auto& element = values[i];
        if (element.value == value) {
            selected = i;
        }
    }
    assert(selected < values.size());

    bool rv{ false };
    if (ImGui::BeginCombo(ss.str().c_str(), values[selected].prettyName.c_str())) {
        for (size_t i = 0; i < values.size(); i++) {
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
    ImGui::TextUnformatted(std::to_string(val).c_str());
    EndWidget();
}

bool Int8Widget(const std::string& label, int8_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_S8, &value);
    EndWidget();
    return rv;
}
bool Int16Widget(const std::string& label, int16_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_S16, &value);
    EndWidget();
    return rv;
}
bool Int32Widget(const std::string& label, int32_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_S32, &value);
    EndWidget();
    return rv;
}
bool Int64Widget(const std::string& label, int64_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_S64, &value);
    EndWidget();
    return rv;
}
bool UInt8Widget(const std::string& label, uint8_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_U8, &value);
    EndWidget();
    return rv;
}
bool UInt16Widget(const std::string& label, uint16_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_U16, &value);
    EndWidget();
    return rv;
}
bool UInt32Widget(const std::string& label, uint32_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_U32, &value);
    EndWidget();
    return rv;
}
bool UInt64Widget(const std::string& label, uint64_t& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_U64, &value);
    EndWidget();
    return rv;
}
bool IntWidget(const std::string& label, int& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragInt(ss.str().c_str(), &value);
    EndWidget();
    return rv;
}
bool LongWidget(const std::string& label, long& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_S64, &value);
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
    bool rv = ImGui::DragScalar(ss.str().c_str(), ImGuiDataType_Double, (void*) &value, 0.1f);
    EndWidget();
    return rv;
}

bool BoolWidget(const std::string& label, bool& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4);
    bool rv = ImGui::Checkbox(ss.str().c_str(), &value);
    ImGui::PopStyleVar();
    EndWidget();
    return rv;
}

bool StringWidget(const std::string& label, std::string& value) {
    BeginWidget(label);
    std::stringstream ss;
    ss << "##" << label;
    std::array<char, 128> buf{};
    std::copy(value.begin(), value.end(), buf.begin());
    bool rv{ false };
    if (ImGui::InputText(ss.str().c_str(), buf.data(), 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
        rv = true;
        value = std::string(buf.data());
    }
    EndWidget();
    return rv;
}

void UneditableStringWidget(const std::string& label, const std::string& value) {
    BeginWidget(label);
    ImGui::TextUnformatted(value.c_str());
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
        ImGui::TextEx(ss.str().c_str());
        ImGui::Spacing();

        ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_HDR |
            ImGuiColorEditFlags_AlphaBar |
            ImGuiColorEditFlags_NoLabel |
            ImGuiColorEditFlags_AlphaPreviewHalf;

        ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 12.0f);
        rv |= ImGui::NeoDoaColorPickerPopup("##picker", value.Data(), picker_flags, &ctx.ColorPickerRef.x);
        ImGui::EndPopup();
    }

    EndWidget();
    return rv;
}

bool ResolutionWidget(const std::string& label, Resolution& resolution) {
    glm::vec2 res{ resolution.Width, resolution.Height };
    FancyVectorWidgetSettings<Display::XY> settingsFBO;
    settingsFBO.resetEnabled = false;
    settingsFBO.speed = 1;
    settingsFBO.min = 128;
    settingsFBO.max = 128 * 128;
    settingsFBO.fmt = "%g";
    settingsFBO.displayLabelOverride[0] = "W";
    settingsFBO.displayLabelOverride[1] = "H";
    settingsFBO.displayLabelColorOverride[0] = Color(0.45f, 0.63f, 0.82f);
    settingsFBO.displayLabelColorOverride[1] = Color(0.66f, 0.49f, 0.65f);

    bool rv = FancyVector2Widget(label, res, settingsFBO);
    resolution = { static_cast<decltype(Resolution::Width)>(res.x), static_cast<decltype(Resolution::Height)>(res.y) };

    return rv;
}

bool OrthoCameraWidget(OrthoCamera& cameraData) {
    glm::vec1 top{ cameraData.TopPlane };
    glm::vec2 leftRight{ cameraData.LeftPlane, cameraData.RightPlane };
    glm::vec1 bottom{ cameraData.BottomPlane };

    FancyVectorWidgetSettings<Display::X> settingsOrthoTop;
    settingsOrthoTop.resetTo = 1;
    settingsOrthoTop.displayLabelOverride[0] = "T";
    settingsOrthoTop.displayLabelColorOverride[0] = Color(0.10f, 0.53f, 0.26f);
    settingsOrthoTop.displayLabelHoverColorOverride[0] = settingsOrthoTop.displayLabelColorOverride[0].Lighten(0.45f);
    settingsOrthoTop.componentFieldWidth /= 2;

    FancyVectorWidgetSettings<Display::XY> settingsOrthoLeftRight;
    settingsOrthoLeftRight.resetAllToSame = false;
    settingsOrthoLeftRight.resetTos[0] = -1;
    settingsOrthoLeftRight.resetTos[1] = 1;
    settingsOrthoLeftRight.displayLabelOverride[0] = "L";
    settingsOrthoLeftRight.displayLabelOverride[1] = "R";
    settingsOrthoLeftRight.displayLabelColorOverride[0] = Color(0.10f, 0.53f, 0.26f);
    settingsOrthoLeftRight.displayLabelColorOverride[1] = Color(0.10f, 0.53f, 0.26f);
    settingsOrthoLeftRight.displayLabelHoverColorOverride[0] = settingsOrthoLeftRight.displayLabelColorOverride[0].Lighten(0.45f);
    settingsOrthoLeftRight.displayLabelHoverColorOverride[1] = settingsOrthoLeftRight.displayLabelColorOverride[1].Lighten(0.45f);

    FancyVectorWidgetSettings<Display::X> settingsOrthoBottom;
    settingsOrthoBottom.resetTo = -1;
    settingsOrthoBottom.displayLabelOverride[0] = "B";
    settingsOrthoBottom.displayLabelColorOverride[0] = Color(0.10f, 0.53f, 0.26f);
    settingsOrthoBottom.displayLabelHoverColorOverride[0] = settingsOrthoBottom.displayLabelColorOverride[0].Lighten(0.45f);
    settingsOrthoBottom.componentFieldWidth /= 2;

    bool rv{ false };
    rv = FancyVector1Widget("Top", top, settingsOrthoTop);
    rv = rv | FancyVector2Widget("Left & Right", leftRight, settingsOrthoLeftRight); // no double pipe because of short circuit shadowing imgui call
    rv = rv | FancyVector1Widget("Bottom", bottom, settingsOrthoBottom);

    cameraData.TopPlane = top.x;
    cameraData.LeftPlane = leftRight.x;
    cameraData.RightPlane = leftRight.y;
    cameraData.BottomPlane = bottom.x;

    return rv;
}

bool PerspectiveCameraWidget(PerspectiveCamera& cameraData) {
    glm::vec1 fov{ cameraData.FOV };
    glm::vec1 aspect{ cameraData.AspectRatio };

    FancyVectorWidgetSettings<Display::X> fovSettings;
    fovSettings.resetTo = 110;
    fovSettings.min = 30;
    fovSettings.max = 150;
    fovSettings.displayLabelOverride[0] = "F";
    fovSettings.displayLabelColorOverride[0] = Color(0.23f, 0.23f, 0.13f);
    fovSettings.displayLabelHoverColorOverride[0] = fovSettings.displayLabelColorOverride[0].Lighten(0.35f);

    FancyVectorWidgetSettings<Display::X> aspectSettings;
    aspectSettings.disabled = true;
    aspectSettings.displayLabelOverride[0] = "A";
    aspectSettings.displayLabelColorOverride[0] = Color(0.211f, 0.121f, 0.439f);

    bool rv{ false };
    rv = FancyVector1Widget("Field Of View", fov, fovSettings);
    rv = rv | FancyVector1Widget("Aspect Ratio", aspect, aspectSettings); // no double pipe because of short circuit shadowing imgui call

    cameraData.FOV = fov.x;

    return rv;
}

bool Image2DButtonWidget(const std::string& label, ImTextureID texture) {
    BeginWidget(label);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 128 - ImGui::GetStyle().FramePadding.x);
    std::stringstream ss;
    ss << "##" << label;
    bool rv = ImGui::ImageButton(label.c_str(), texture, { 128, 128 }, { 0, 1 }, { 1, 0 });
    EndWidget();
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

    ImGui::TextUnformatted(label.c_str());
    ImGui::Separator();

    ImGui::PopFont();
    ImGui::PopID();
}

void Separator() {
    ImGui::NewLine();
    ImGui::Separator();
}

// Modified ColorPicker4()
// Displays FancyVectorPieces instead of DragFloats
bool ImGui::NeoDoaColorPickerPopup(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col) {
    using namespace ImGui;

    // ColorEdit supports RGB and HSV inputs. In case of RGB input resulting color may have undefined hue and/or saturation.
    // Since widget displays both RGB and HSV values we must preserve hue and saturation to prevent these values resetting.
    auto ColorEditRestoreHS = [](const float* col, float* H, float* S, float* V) {
        // This check is optional. Suppose we have two color widgets side by side, both widgets display different colors, but both colors have hue and/or saturation undefined.
        // With color check: hue/saturation is preserved in one widget. Editing color in one widget would reset hue/saturation in another one.
        // Without color check: common hue/saturation would be displayed in all widgets that have hue/saturation undefined.
        // g.ColorEditSavedColor is stored as ImU32 RGB value: this essentially gives us color equality check with reduced precision.
        // Tiny external color changes would not be detected and this check would still pass. This is OK, since we only restore hue/saturation _only_ if they are undefined,
        // therefore this change flipping hue/saturation from undefined to a very tiny value would still be represented in color picker.
        ImGuiContext& g = *GImGui;
        if (g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) {
            return;
        }

        // When S == 0, H is undefined.
        // When H == 1 it wraps around to 0.
        if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1)) {
            *H = g.ColorEditSavedHue;
        }

        // When V == 0, S is undefined.
        if (*V == 0.0f) {
            *S = g.ColorEditSavedSat;
        }
    };

    // Helper for NeoDoaColorPickerPopup()
    auto RenderArrowsForVerticalBar = [](ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha) {
        ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) {
        return false;
    }

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    g.NextItemData.ClearFlags();

    PushID(label);
    BeginGroup();

    if (!(flags & ImGuiColorEditFlags_NoSidePreview)) {
        flags |= ImGuiColorEditFlags_NoSmallPreview;
    }

    // Context menu: display and store options.
    if (!(flags & ImGuiColorEditFlags_NoOptions)) {
        ColorPickerOptionsPopup(col, flags);
    }

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_PickerMask_)) {
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    }
    if (!(flags & ImGuiColorEditFlags_InputMask_)) {
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
    }
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
    if (!(flags & ImGuiColorEditFlags_NoOptions)) {
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);
    }

    // Setup
    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
    ImVec2 picker_pos = window->DC.CursorPos;
    float square_sz = GetFrameHeight();
    float bars_width = square_sz; // Arbitrary smallish width of Hue/Alpha picking bars
    float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
    float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
    float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
    float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float wheel_thickness = sv_picker_size * 0.08f;
    float wheel_r_outer = sv_picker_size * 0.50f;
    float wheel_r_inner = wheel_r_outer - wheel_thickness;
    ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

    // Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
    float triangle_r = wheel_r_inner - (int) (sv_picker_size * 0.027f);
    ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
    ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
    ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB) {
        // Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        ColorEditRestoreHS(col, &H, &S, &V);
    } else if (flags & ImGuiColorEditFlags_InputHSV) {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (flags & ImGuiColorEditFlags_PickerHueWheel) {
        // Hue wheel + SV triangle logic
        InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
        if (IsItemActive()) {
            ImVec2 initial_off = { g.IO.MouseClickedPos[0].x - wheel_center.x, g.IO.MouseClickedPos[0].y - wheel_center.y };
            ImVec2 current_off = { g.IO.MousePos.x - wheel_center.x, g.IO.MousePos.y - wheel_center.y };
            float initial_dist2 = ImLengthSqr(initial_off);
            if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1)) {
                // Interactive with Hue wheel
                H = ImAtan2(current_off.y, current_off.x) / IM_PI * 0.5f;
                if (H < 0.0f) {
                    H += 1.0f;
                }
                value_changed = value_changed_h = true;
            }
            float cos_hue_angle = ImCos(-H * 2.0f * IM_PI);
            float sin_hue_angle = ImSin(-H * 2.0f * IM_PI);
            if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle))) {
                // Interacting with SV triangle
                ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
                if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated)) {
                    current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
                }
                float uu, vv, ww;
                ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
                V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
                S = ImClamp(uu / V, 0.0001f, 1.0f);
                value_changed = value_changed_sv = true;
            }
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions)) {
            OpenPopupOnItemClick("context");
        }
    } else if (flags & ImGuiColorEditFlags_PickerHueBar) {
        // SV rectangle logic
        InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
        if (IsItemActive()) {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));

            // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
            if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) {
                H = g.ColorEditSavedHue;
            }
            value_changed = value_changed_sv = true;
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions)) {
            OpenPopupOnItemClick("context");
        }

        // Hue bar logic
        SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
        InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive()) {
            H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_h = true;
        }
    }

    // Alpha bar logic
    if (alpha_bar) {
        SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
        InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive()) {
            col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = true;
        }
    }
    PopItemFlag(); // ImGuiItemFlags_NoNav

    if (!(flags & ImGuiColorEditFlags_NoSidePreview)) {
        SameLine(0, style.ItemInnerSpacing.x);
        BeginGroup();
    }

    if (!(flags & ImGuiColorEditFlags_NoLabel)) {
        const char* label_display_end = FindRenderedTextEnd(label);
        if (label != label_display_end) {
            if ((flags & ImGuiColorEditFlags_NoSidePreview)) {
                SameLine(0, style.ItemInnerSpacing.x);
            }
            TextEx(label, label_display_end);
        }
    }

    if (!(flags & ImGuiColorEditFlags_NoSidePreview)) {
        PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
        ImVec4 col_v4(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
        if ((flags & ImGuiColorEditFlags_NoLabel)) {
            Text("Current");
        }

        ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip;
        ColorButton("##current", col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2));
        if (ref_col != NULL) {
            Text("Original");
            ImVec4 ref_col_v4(ref_col[0], ref_col[1], ref_col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : ref_col[3]);
            if (ColorButton("##original", ref_col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2))) {
                memcpy(col, ref_col, components * sizeof(float));
                value_changed = true;
            }
        }
        PopItemFlag();
        EndGroup();
    }

    // Convert back color to RGB
    if (value_changed_h || value_changed_sv) {
        if (flags & ImGuiColorEditFlags_InputRGB) {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        } else if (flags & ImGuiColorEditFlags_InputHSV) {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    // R,G,B and H,S,V slider color editor
    bool value_changed_fix_hue_wrap = false;
    if ((flags & ImGuiColorEditFlags_NoInputs) == 0) {
        PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
        if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0) {
            FancyVectorWidgetSettings<Display::XYZW> settings;
            settings.fmt = "%g";
            settings.speed = 1.0f;
            settings.min = 0.0f;
            settings.max = 255.0f;
            settings.componentFieldWidth = static_cast<int>(CalcItemWidth());
            settings.displayLabelOverride[0] = "R";
            settings.displayLabelOverride[1] = "G";
            settings.displayLabelOverride[2] = "B";
            settings.displayLabelOverride[3] = "A";
            settings.displayLabelID[0] = "###RGBR";
            settings.displayLabelID[1] = "###RGBG";
            settings.displayLabelID[2] = "###RGBB";
            settings.displayLabelID[3] = "###RGBA";
            settings.scaleY[3] = 2;

            float denorm[4] = {
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(col[0])),
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(col[1])),
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(col[2])),
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(col[3]))
            };

            value_changed |= FancyVectorPiece<Display::XYZW>(settings, 0, denorm);
            SameLine();
            value_changed |= FancyVectorPiece<Display::XYZW>(settings, 1, denorm);
            SameLine();
            value_changed |= FancyVectorPiece<Display::XYZW>(settings, 2, denorm);
            SameLine();
            value_changed |= FancyVectorPiece<Display::XYZW>(settings, 3, denorm);

            if (value_changed) {
                // FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using the InputText or DropTarget.
                // For the later we don't want to run the hue-wrap canceling code. If you are well versed in HSV picker please provide your input! (See #2050)
                col[0] = std::clamp(denorm[0] / 255, 0.0f, 255.0f);
                col[1] = std::clamp(denorm[1] / 255, 0.0f, 255.0f);
                col[2] = std::clamp(denorm[2] / 255, 0.0f, 255.0f);
                col[3] = std::clamp(denorm[3] / 255, 0.0f, 255.0f);
                value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
                value_changed = true;
            }
        }
        if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0) {
            FancyVectorWidgetSettings<Display::XYZW> settings;
            settings.fmt = "%g";
            settings.speed = 1.0f;
            settings.min = 0.0f;
            settings.max = 255.0f;
            settings.componentFieldWidth = static_cast<int>(CalcItemWidth());
            settings.displayLabelOverride[0] = "H";
            settings.displayLabelOverride[1] = "S";
            settings.displayLabelOverride[2] = "V";
            settings.displayLabelOverride[3] = "A";
            settings.displayLabelID[0] = "###HSVR";
            settings.displayLabelID[1] = "###HSVG";
            settings.displayLabelID[2] = "###HSVB";
            settings.displayLabelID[3] = "###HSVA";
            settings.displayLabelColorOverride[0] = Color(0.78f, 0.60f, 0.12f);
            settings.displayLabelColorOverride[1] = Color(0.42f, 0.42f, 0.42f);
            settings.displayLabelColorOverride[2] = Color(0.11f, 0.11f, 0.11f);

            float f[4] = { col[0], col[1], col[2], col[3] };
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
            ColorEditRestoreHS(col, &f[0], &f[1], &f[2]);

            float denorm[4] = {
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(f[0])),
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(f[1])),
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(f[2])),
                static_cast<float>(IM_F32_TO_INT8_UNBOUND(f[3]))
            };

            value_changed |= FancyVectorPiece(settings, 0, denorm);
            SameLine();
            value_changed |= FancyVectorPiece(settings, 1, denorm);
            SameLine();
            value_changed |= FancyVectorPiece(settings, 2, denorm);
            //SameLine();
            //value_changed |= FancyVectorPiece(settings, 3, denorm);

            if (value_changed) {
                for (int n = 0; n < 4; n++) {
                    f[n] = denorm[n] / 255.0f;
                }

                g.ColorEditSavedHue = f[0];
                g.ColorEditSavedSat = f[1];
                ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
                g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
                col[0] = f[0];
                col[1] = f[1];
                col[2] = f[2];
                col[3] = f[3];
            }
        }
        if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0) {
            int denorm[4] = {
                IM_F32_TO_INT8_UNBOUND(col[0]),
                IM_F32_TO_INT8_UNBOUND(col[1]),
                IM_F32_TO_INT8_UNBOUND(col[2]),
                IM_F32_TO_INT8_UNBOUND(col[3])
            };

            char buf[64];
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(denorm[0], 0, 255), ImClamp(denorm[1], 0, 255), ImClamp(denorm[2], 0, 255), ImClamp(denorm[3], 0, 255));
            SetNextItemWidth(CalcItemWidth());
            if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase)) {
                value_changed = true;
                char* p = buf;
                while (*p == '#' || ImCharIsBlankA(*p))
                    p++;
                denorm[0] = denorm[1] = denorm[2] = 0;
                denorm[3] = 0xFF; // alpha default to 255 is not parsed by scanf (e.g. inputting #FFFFFF omitting alpha)
                sscanf(p, "%02X%02X%02X%02X", (unsigned int*) &denorm[0], (unsigned int*) &denorm[1], (unsigned int*) &denorm[2], (unsigned int*) &denorm[3]);
                col[0] = denorm[0] / 255.0f;
                col[1] = denorm[1] / 255.0f;
                col[2] = denorm[2] / 255.0f;
                col[3] = denorm[3] / 255.0f;
            }

            //value_changed |= ColorEdit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);
        }
        PopItemWidth();
    }

    // Try to cancel hue wrap (after ColorEdit4 call), if any
    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB)) {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0) {
            if (new_V <= 0 && V != new_V) {
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            } else if (new_S <= 0) {
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
            }
        }
    }

    if (value_changed) {
        if (flags & ImGuiColorEditFlags_InputRGB) {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            ColorEditRestoreHS(col, &H, &S, &V);   // Fix local Hue as display below will use it immediately.
        } else if (flags & ImGuiColorEditFlags_InputHSV) {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

    ImVec2 sv_cursor_pos;

    if (flags & ImGuiColorEditFlags_PickerHueWheel) {
        // Render Hue Wheel
        const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
        const int segment_per_arc = ImMax(4, (int) wheel_r_outer / 12);
        for (int n = 0; n < 6; n++) {
            const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
            const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
            const int vert_start_idx = draw_list->VtxBuffer.Size;
            draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
            draw_list->PathStroke(col_white, 0, wheel_thickness);
            const int vert_end_idx = draw_list->VtxBuffer.Size;

            // Paint colors over existing vertices
            ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
            ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
            ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
        }

        // Render Cursor + preview on Hue Wheel
        float cos_hue_angle = ImCos(H * 2.0f * IM_PI);
        float sin_hue_angle = ImSin(H * 2.0f * IM_PI);
        ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
        float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
        int hue_cursor_segments = ImClamp((int) (hue_cursor_rad / 1.4f), 9, 32);
        draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
        draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, col_midgrey, hue_cursor_segments);
        draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, col_white, hue_cursor_segments);

        // Render SV triangle (rotated according to hue)
        ImVec2 rota = ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
        ImVec2 rotb = ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
        ImVec2 rotc = ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
        ImVec2 tra = { wheel_center.x + rota.x, wheel_center.y + rota.y };
        ImVec2 trb = { wheel_center.x + rotb.x, wheel_center.y + rotb.y };
        ImVec2 trc = { wheel_center.x + rotc.x, wheel_center.y + rotc.y };
        ImVec2 uv_white = GetFontTexUvWhitePixel();
        draw_list->PrimReserve(6, 6);
        draw_list->PrimVtx(tra, uv_white, hue_color32);
        draw_list->PrimVtx(trb, uv_white, hue_color32);
        draw_list->PrimVtx(trc, uv_white, col_white);
        draw_list->PrimVtx(tra, uv_white, 0);
        draw_list->PrimVtx(trb, uv_white, col_black);
        draw_list->PrimVtx(trc, uv_white, 0);
        draw_list->AddTriangle(tra, trb, trc, col_midgrey, 1.5f);
        sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
    } else if (flags & ImGuiColorEditFlags_PickerHueBar) {
        // Render SV Square
        ImVec2 p_max = { picker_pos.x + sv_picker_size, picker_pos.y + sv_picker_size };
        draw_list->AddRectFilledMultiColor(picker_pos, p_max, col_white, hue_color32, hue_color32, col_white);
        draw_list->AddRectFilledMultiColor(picker_pos, p_max, 0, 0, col_black, col_black);
        RenderFrameBorder(picker_pos, p_max, 0.0f);
        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

        // Render Hue Bar
        for (int i = 0; i < 6; ++i)
            draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
        float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
        RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
        RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
    }

    // Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
    float sv_cursor_rad = value_changed_sv ? 10.0f : 6.0f;
    draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, 12);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, 12);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, 12);

    // Render alpha bar
    if (alpha_bar) {
        float alpha = ImSaturate(col[3]);
        ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
        RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
        draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
        float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
        RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
        RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
    }

    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0) {
        value_changed = false;
    }
    if (value_changed) {
        MarkItemEdited(g.LastItemData.ID);
    }

    PopID();

    return value_changed;
}
