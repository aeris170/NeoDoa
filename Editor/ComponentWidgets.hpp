#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

#include <Engine/NeoDoa.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Strings.hpp>
#include <Editor/AssetFilter.hpp>
#include <Editor/MetaAssetInfo.hpp>

inline constexpr int compFieldWidth = 300; // must be divisible by both 3 and 4 (and 1 and 2, but you know... MaThS...) (x,y,z,w)

enum class Display {
    X = 1, XY = 2, XYZ = 3, XYZW = 4
};

template<Display dsp>
struct FancyVectorWidgetSettings {
    bool disabled{ false };
    bool resetEnabled{ true };
    bool resetAllToSame{ true };
    float resetTo{ 0.0f };
    std::array<float, static_cast<size_t>(dsp)> resetTos{};
    Display display{ dsp };
    float speed{ 0.1f };
    float min{ 0.0f };
    float max{ 0.0f };
    const char* fmt{ "%.3f" };
    int componentFieldWidth{ compFieldWidth };
    bool justifyCenter{ true };
    std::array<const char*, static_cast<size_t>(dsp)> displayLabelOverride{};
    std::array<Color, static_cast<size_t>(dsp)> displayLabelColorOverride{};
    std::array<Color, static_cast<size_t>(dsp)> displayLabelHoverColorOverride{};
    std::array<const char*, static_cast<size_t>(dsp)> displayLabelID{};
    std::array<int, static_cast<size_t>(dsp)> scaleY{};

    constexpr FancyVectorWidgetSettings() {
        displayLabelOverride[0] = "X";
        displayLabelColorOverride[0] = Color(0.80f, 0.10f, 0.15f);
        displayLabelHoverColorOverride[0] = Color(0.94f, 0.51f, 0.55f);
        displayLabelID[0] = "###X";
        scaleY[0] = 1;

        if constexpr (dsp == Display::XY) {
            displayLabelOverride[1] = "Y";
            displayLabelColorOverride[1] = Color(0.20f, 0.70f, 0.20f);
            displayLabelHoverColorOverride[1] = Color(0.57f, 0.87f, 0.57f);
            displayLabelID[1] = "###Y";
            scaleY[1] = 1;
        }

        if constexpr (dsp == Display::XYZ) {
            displayLabelOverride[1] = "Y";
            displayLabelColorOverride[1] = Color(0.20f, 0.70f, 0.20f);
            displayLabelHoverColorOverride[1] = Color(0.57f, 0.87f, 0.57f);
            displayLabelID[1] = "###Y";
            scaleY[1] = 1;

            displayLabelOverride[2] = "Z";
            displayLabelColorOverride[2] = Color(0.10f, 0.25f, 0.80f);
            displayLabelHoverColorOverride[2] = Color(0.51f, 0.60f, 0.94f);
            displayLabelID[2] = "###Z";
            scaleY[2] = 1;
        }

        if constexpr (dsp == Display::XYZW) {
            displayLabelOverride[1] = "Y";
            displayLabelColorOverride[1] = Color(0.20f, 0.70f, 0.20f);
            displayLabelHoverColorOverride[1] = Color(0.57f, 0.87f, 0.57f);
            displayLabelID[1] = "###Y";
            scaleY[1] = 1;

            displayLabelOverride[2] = "Z";
            displayLabelColorOverride[2] = Color(0.10f, 0.25f, 0.80f);
            displayLabelHoverColorOverride[2] = Color(0.51f, 0.60f, 0.94f);
            displayLabelID[2] = "###Z";
            scaleY[2] = 1;

            displayLabelOverride[3] = "W";
            displayLabelColorOverride[3] = Color(0.61f, 0.29f, 0.85f);
            displayLabelHoverColorOverride[3] = Color(0.80f, 0.64f, 0.93f);
            displayLabelID[3] = "###W";
            scaleY[3] = 1;
        }
    }
};
inline constexpr FancyVectorWidgetSettings<Display::X> defaultFancyVectorSettingsX;
inline constexpr FancyVectorWidgetSettings<Display::XY> defaultFancyVectorSettingsXY;
inline constexpr FancyVectorWidgetSettings<Display::XYZ> defaultFancyVectorSettingsXYZ;
inline constexpr FancyVectorWidgetSettings<Display::XYZW> defaultFancyVectorSettingsXYZW;

bool FancyVector1Widget(const std::string& label, glm::vec1& vec, FancyVectorWidgetSettings<Display::X> settings = defaultFancyVectorSettingsX);
bool FancyVector2Widget(const std::string& label, glm::vec2& vec, FancyVectorWidgetSettings<Display::XY> settings = defaultFancyVectorSettingsXY);
bool FancyVector3Widget(const std::string& label, glm::vec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings = defaultFancyVectorSettingsXYZ);
bool FancyVector4Widget(const std::string& label, glm::vec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings = defaultFancyVectorSettingsXYZW);

bool FancyVectori1Widget(const std::string& label, glm::ivec1& vec, FancyVectorWidgetSettings<Display::X> settings = defaultFancyVectorSettingsX);
bool FancyVectori2Widget(const std::string& label, glm::ivec2& vec, FancyVectorWidgetSettings<Display::XY> settings = defaultFancyVectorSettingsXY);
bool FancyVectori3Widget(const std::string& label, glm::ivec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings = defaultFancyVectorSettingsXYZ);
bool FancyVectori4Widget(const std::string& label, glm::ivec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings = defaultFancyVectorSettingsXYZW);

bool FancyVectorui1Widget(const std::string& label, glm::uvec1& vec, FancyVectorWidgetSettings<Display::X> settings = defaultFancyVectorSettingsX);
bool FancyVectorui2Widget(const std::string& label, glm::uvec2& vec, FancyVectorWidgetSettings<Display::XY> settings = defaultFancyVectorSettingsXY);
bool FancyVectorui3Widget(const std::string& label, glm::uvec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings = defaultFancyVectorSettingsXYZ);
bool FancyVectorui4Widget(const std::string& label, glm::uvec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings = defaultFancyVectorSettingsXYZW);

bool FancyVectorb1Widget(const std::string& label, glm::ivec1& vec, FancyVectorWidgetSettings<Display::X> settings = defaultFancyVectorSettingsX);
bool FancyVectorb2Widget(const std::string& label, glm::ivec2& vec, FancyVectorWidgetSettings<Display::XY> settings = defaultFancyVectorSettingsXY);
bool FancyVectorb3Widget(const std::string& label, glm::ivec3& vec, FancyVectorWidgetSettings<Display::XYZ> settings = defaultFancyVectorSettingsXYZ);
bool FancyVectorb4Widget(const std::string& label, glm::ivec4& vec, FancyVectorWidgetSettings<Display::XYZW> settings = defaultFancyVectorSettingsXYZW);

template<Display dsp>
bool FancyVectorPiece(FancyVectorWidgetSettings<dsp>& settings, size_t idx, float* vec, ImFont* buttonFont = nullptr, ImVec2 buttonSize = { 0, 0 }) {
    bool rv{ false };
    ImGui::PushID(settings.displayLabelID[idx]);

    if (buttonFont == nullptr) {
        buttonFont = ImGui::GetIO().Fonts->Fonts[1];
    }
    float lineHeight = buttonFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    if (buttonSize.x == 0 && buttonSize.y == 0) {
        buttonSize = { lineHeight + 3.0f, lineHeight * settings.scaleY[idx] };
    }

    Color btnColor = settings.displayLabelColorOverride[idx];
    Color btnHoverColor = settings.displayLabelHoverColorOverride[idx];
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ btnColor.r, btnColor.g, btnColor.b, btnColor.a });
    if (!settings.disabled && settings.resetEnabled) {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ btnHoverColor.r, btnHoverColor.g, btnHoverColor.b, btnHoverColor.a });
    } else {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ btnColor.r, btnColor.g, btnColor.b, btnColor.a });
    }
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ btnColor.r, btnColor.g, btnColor.b, btnColor.a });
    ImGui::PushFont(buttonFont);
    ImVec2 textSize = ImGui::CalcTextSize(settings.displayLabelOverride[idx]);
    if (ImGui::Button(settings.displayLabelOverride[idx], buttonSize)) {
        if (!settings.disabled) {
            if (settings.resetEnabled) {
                rv = true;
                if (settings.resetAllToSame) {
                    vec[idx] = settings.resetTo;
                } else {
                    vec[idx] = settings.resetTos[idx];
                }
            }
        }
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    ImGui::PushItemWidth(settings.componentFieldWidth / static_cast<int>(dsp) - buttonSize.x);

    if (settings.disabled) { ImGui::BeginDisabled(); }
    float y = ImGui::GetStyle().FramePadding.y;
    if (settings.scaleY[idx] > 1) {
        ImGui::GetStyle().FramePadding.y += (lineHeight + y) * (settings.scaleY[idx] - 1);
        ImGui::GetStyle().FramePadding.y /= 2; // FramePadding.y gets multiplied by 2 inside DragFloat, so we divide by 2 here
    }

    float startY = ImGui::GetCursorPosY() + lineHeight; // save maxY, because scaleY fucks up next line start pos

    if (ImGui::DragFloat(std::string("##").append(settings.displayLabelOverride[idx]).c_str(), &(vec[idx]), settings.speed, settings.min, settings.max, settings.fmt)) {
        rv = true;
    }

    ImGui::SetCursorPosY(startY); // restore maxY

    ImGui::GetStyle().FramePadding.y = y;
    if (settings.disabled) { ImGui::EndDisabled(); }
    ImGui::PopItemWidth();

    ImGui::PopID();
    return rv;
}

template<Display dsp>
bool FancyVectorWidget(const std::string& label, float* vec, FancyVectorWidgetSettings<dsp> settings = defaultFancyVectorSettingsXYZW) {
    bool rv{ false };

    float w = ImGui::GetContentRegionAvail().x;

    ImGui::PushItemWidth(static_cast<float>(settings.componentFieldWidth));

    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2, nullptr, false);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0 });
    ImGui::SetColumnWidth(0, w - compFieldWidth);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);
    ImGui::Text("%s", label.c_str());
    if (ImGui::BeginPopupContextItem(label.c_str(), ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem(cat(ComponentWidgetIcons::ContextMenu::ResetIcon, ComponentWidgetStrings::ContextMenu::Reset))) {
            rv = true;
            if (settings.resetAllToSame) {
                vec[0] = settings.resetTo;
                if constexpr (dsp == Display::XY) {
                    vec[1] = settings.resetTo;
                }
                if constexpr (dsp == Display::XYZ) {
                    vec[1] = settings.resetTo;
                    vec[2] = settings.resetTo;
                }
                if constexpr (dsp == Display::XYZW) {
                    vec[1] = settings.resetTo;
                    vec[2] = settings.resetTo;
                    vec[3] = settings.resetTo;
                }
            } else {
                vec[0] = settings.resetTos[0];
                if constexpr (dsp == Display::XY) {
                    vec[1] = settings.resetTos[1];
                }
                if constexpr (dsp == Display::XYZ) {
                    vec[1] = settings.resetTos[1];
                    vec[2] = settings.resetTos[2];
                }
                if constexpr (dsp == Display::XYZW) {
                    vec[1] = settings.resetTos[1];
                    vec[2] = settings.resetTos[2];
                    vec[3] = settings.resetTos[3];
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::NextColumn();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);
    if (settings.justifyCenter) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (compFieldWidth - settings.componentFieldWidth) / 2);
    }

    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    rv |= FancyVectorPiece(settings, 0, vec, boldFont, buttonSize);
    if constexpr (dsp == Display::XY) {
        ImGui::SameLine();
        rv |= FancyVectorPiece(settings, 1, vec, boldFont, buttonSize);
    }
    if constexpr (dsp == Display::XYZ) {
        ImGui::SameLine();
        rv |= FancyVectorPiece(settings, 1, vec, boldFont, buttonSize);
        ImGui::SameLine();
        rv |= FancyVectorPiece(settings, 2, vec, boldFont, buttonSize);
    }
    if constexpr (dsp == Display::XYZW) {
        ImGui::SameLine();
        rv |= FancyVectorPiece(settings, 1, vec, boldFont, buttonSize);
        ImGui::SameLine();
        rv |= FancyVectorPiece(settings, 2, vec, boldFont, buttonSize);
        ImGui::SameLine();
        rv |= FancyVectorPiece(settings, 3, vec, boldFont, buttonSize);
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

bool Int8Widget(const std::string& label, int8_t& value);
bool Int16Widget(const std::string& label, int16_t& value);
bool Int32Widget(const std::string& label, int32_t& value);
bool Int64Widget(const std::string& label, int64_t& value);
bool UInt8Widget(const std::string& label, uint8_t& value);
bool UInt16Widget(const std::string& label, uint16_t& value);
bool UInt32Widget(const std::string& label, uint32_t& value);
bool UInt64Widget(const std::string& label, uint64_t& value);
bool IntWidget(const std::string& label, int& value);
bool LongWidget(const std::string& label, long& value);

bool FloatWidget(const std::string& label, float& value, float step = 0.1f, float min = 0.0f, float max = 0.0f);

bool DoubleWidget(const std::string& label, double& value);

bool BoolWidget(const std::string& label, bool& value);

bool StringWidget(const std::string& label, std::string& value);

void UneditableStringWidget(const std::string& label, const std::string& value);

bool ColorWidget(const std::string& label, Color& value);

bool ResolutionWidget(const std::string& label, Resolution& resolution);

bool OrthoCameraWidget(OrthoCamera& cameraData);

bool PerspectiveCameraWidget(PerspectiveCamera& cameraData);

bool Image2DButtonWidget(const std::string& label, ImTextureID texture);

void Space();

void Header(const std::string& label);

void Separator();

namespace ImGui {
    bool NeoDoaColorPickerPopup(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col);
}

using namespace detail;

template<typename T>
    requires requires (T x) { std::to_string(x); }
void UneditableArrayWidget(const std::string& label, const std::vector<T>& array) {
    BeginWidget(label);
    for (int i = 0; i < array.size(); i++) {
        ImGui::Text("%s", std::to_string(array[i]).c_str());
    }
    EndWidget();
}

template<AssetFilter Filter>
bool MultiAssetWidget(std::string_view label, std::vector<UUID>& uuids, const Assets& assets, MetaAssetInfoBank& metaBank, Filter filter) {
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight, lineHeight };
    float margin = 3.0f;

    for (int i = 0; i < uuids.size(); i++) {
        UUID id = uuids[i];

        BeginWidget(std::format("{}[{}]: ", label.data(), i));

        ImVec4 textColor;
        std::string_view assetIcon;
        std::string_view assetName;
        if (id != UUID::Empty()) {
            AssetHandle handle = assets.FindAsset(id);
            if (handle.HasValue()) {
                textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
                assetIcon = metaBank.GetMetaInfoOf(handle->File()).fa_icon;
                assetName = handle->File().Name();
            } else {
                textColor = MultiAssetWidgetColors::DeletedOrUnloadedColor;
                assetIcon = ICON_FA_CIRCLE_EXCLAMATION;
                assetName = "Deleted or unloaded";
            }
        } else {
            textColor = MultiAssetWidgetColors::EmptyColor;
            assetIcon = ICON_FA_TRIANGLE_EXCLAMATION;
            assetName = "Empty";
        }

        std::string name{ std::format("{} {} (UUID: {})", assetIcon, assetName.data(), id.AsString()) };

        float x = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(compFieldWidth - margin - buttonSize.x);
        float oldDisabledAlpha = ImGui::GetStyle().DisabledAlpha;
        ImGui::GetStyle().DisabledAlpha = 1.0f;
        ImGui::BeginDisabled();
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::InputText("", name.data(), name.size(), ImGuiInputTextFlags_ReadOnly); // TODO accept drag-drop if (filter(dropped, assets))
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                UUID data = *(const UUID*) payload->Data;
                AssetHandle handle = assets.FindAsset(data);
                assert(handle.HasValue());
                if (filter(data, assets)) {
                    uuids[i] = data;
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
        ImGui::GetStyle().DisabledAlpha = oldDisabledAlpha;
        x += compFieldWidth - buttonSize.x;

        ImGui::PushFont(boldFont);
        ImGui::SameLine();
        ImGui::SetCursorPosX(x);
        ImGui::Button(ICON_FA_BULLSEYE_POINTER, buttonSize); // TODO open popup. pass filter

        ImGui::PopFont();
        EndWidget();
    }
    BeginWidget("");
    ImGui::PushFont(boldFont);
    float x = ImGui::GetCursorPosX() + compFieldWidth - 2 * (buttonSize.x) - margin;

    ImGui::SameLine();
    ImGui::SetCursorPosX(x);
    bool isEmpty = uuids.empty();
    ImGui::BeginDisabled(isEmpty);
    if (ImGui::Button(ICON_FA_MINUS, buttonSize)) {
        uuids.pop_back();
    }
    ImGui::EndDisabled();
    x += margin + buttonSize.x;

    ImGui::SameLine();
    ImGui::SetCursorPosX(x);
    if (ImGui::Button(ICON_FA_PLUS_LARGE, buttonSize)) {
        uuids.push_back(UUID::Empty());
    }
    x += margin + buttonSize.x;

    ImGui::PopFont();
    EndWidget();
    return true;
}