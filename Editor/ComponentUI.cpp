#include "ComponentUI.hpp"

#include <unordered_map>

#include <nameof.hpp>
#include <prettify.hpp>

#include <Scene.hpp>
#include <IDComponent.hpp>
#include <TransformComponent.hpp>
#include <ParentComponent.hpp>
#include <ChildComponent.hpp>
#include <CameraComponent.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "Observer.hpp"
#include "ComponentWidgets.hpp"
#include "UserDefinedComponentStorage.hpp"

void IDComponentUI::Render(const IDComponent& idComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(IDComponent::entity), Prettify(nameof(IDComponent::entity)) },
        { nameof(IDComponent::tag), Prettify(nameof(IDComponent::tag)) }
    };

    IDComponent& id = const_cast<IDComponent&>(idComponent);
    UneditableEntityWidget(UINames[nameof_c(IDComponent::entity)], idComponent.GetEntity());
    StringWidget(UINames[nameof_c(IDComponent::tag)], id.GetTagRef());
}

void TransformComponentUI::Render(const TransformComponent& transformComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(TransformComponent::localTranslation), Prettify(nameof(TransformComponent::localTranslation)) },
        { nameof(TransformComponent::localRotation), Prettify(nameof(TransformComponent::localRotation)) },
        { nameof(TransformComponent::localScale), Prettify(nameof(TransformComponent::localScale)) }
    };

    TransformComponent& transform = const_cast<TransformComponent&>(transformComponent);
    // translation
    {
        glm::vec3 translation = transform.GetLocalTranslation();
        FancyVector3Widget(UINames[nameof(TransformComponent::localTranslation)], translation);
        transform.SetLocalTranslation(translation);
    }
    // rotation
    {
        glm::quat quat = transform.GetLocalRotation();
        glm::vec3 eulersDeg(glm::degrees(glm::eulerAngles(quat)));
        glm::vec3 old(eulersDeg);
        FancyVector3Widget(UINames[nameof(TransformComponent::localRotation)], eulersDeg);
        quat = quat * glm::quat(glm::radians(eulersDeg - old));
        transform.SetLocalRotation(quat);
    }
    // scale
    {
        glm::vec3 scale = transform.GetLocalScale();
        FancyVectorWidgetSettings<display::XYZ> settings;
        settings.resetTo = 1.0f;
        FancyVector3Widget(UINames[nameof(TransformComponent::localScale)], scale, settings);
        transform.SetLocalScale(scale);
    }
}

void ParentComponentUI::Render(const ParentComponent& parentComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(ParentComponent::children), Prettify(nameof(ParentComponent::children)) }
    };

    ParentComponent& parent = const_cast<ParentComponent&>(parentComponent);
    UneditableArrayWidget<Entity>(UINames[nameof(ParentComponent::children)], parent.GetChildren());
}

void ChildComponentUI::Render(const ChildComponent& childComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(ChildComponent::parent), Prettify(nameof(ChildComponent::parent)) }
    };

    ChildComponent& child = const_cast<ChildComponent&>(childComponent);
    UneditableEntityWidget(UINames[nameof(ChildComponent::parent)], childComponent.GetParent());
}

void OrthoCameraComponentUI::Render(const OrthoCameraComponent& orthoCameraComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(OrthoCameraComponent::isActiveAndRendering), Prettify(nameof(OrthoCameraComponent::isActiveAndRendering)) },
        { nameof(OrthoCameraComponent::data), Prettify("orthoCameraProperties") },
        { nameof(FrameBuffer::ClearColor), Prettify("clearColor") },
        { nameof(OrthoCameraComponent::frameBuffer), Prettify("resolution") }
    };

    OrthoCameraComponent& orthoCamera = const_cast<OrthoCameraComponent&>(orthoCameraComponent);
    bool active = orthoCamera.IsActiveAndRendering();
    if (BoolWidget(UINames[nameof(OrthoCameraComponent::isActiveAndRendering)], active)) {
        if (active) { orthoCamera.TurnOn(); } else { orthoCamera.TurnOff(); }
    }

    Space();
    Header(UINames[nameof(OrthoCameraComponent::data)]);

    ColorWidget(UINames[nameof(FrameBuffer::ClearColor)], orthoCamera.GetFrameBuffer().ClearColor);

    Resolution r = orthoCamera.GetFrameBuffer()._resolution;
    if (ResolutionWidget(UINames[nameof(OrthoCameraComponent::frameBuffer)], r)) {
        orthoCamera.SetUpFrameBuffer(r);
    }

    Space();

    OrthoCamera cam = orthoCamera.GetData();
    if (OrthoCameraWidget(cam)) {
        orthoCamera.SetData(std::move(cam));
    }
}

void PerspectiveCameraComponentUI::Render(const PerspectiveCameraComponent& perspectiveCameraComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(PerspectiveCameraComponent::isActiveAndRendering), Prettify(nameof(PerspectiveCameraComponent::isActiveAndRendering)) },
        { nameof(PerspectiveCameraComponent::data), Prettify("orthoCameraProperties") },
        { nameof(FrameBuffer::ClearColor), Prettify("clearColor") },
        { nameof(PerspectiveCameraComponent::frameBuffer), Prettify("resolution") }
    };

    PerspectiveCameraComponent& perspectiveCamera = const_cast<PerspectiveCameraComponent&>(perspectiveCameraComponent);
    bool active = perspectiveCamera.IsActiveAndRendering();
    if (BoolWidget(UINames[nameof(PerspectiveCameraComponent::isActiveAndRendering)], active)) {
        if (active) { perspectiveCamera.TurnOn(); } else { perspectiveCamera.TurnOff(); }
    }

    Space();
    Header(UINames[nameof(PerspectiveCameraComponent::data)]);

    ColorWidget(UINames[nameof(PerspectiveCameraComponent::ClearColor)], perspectiveCamera.GetFrameBuffer().ClearColor);

    Resolution r = perspectiveCamera.GetFrameBuffer()._resolution;
    if (ResolutionWidget(UINames[nameof(PerspectiveCameraComponent::frameBuffer)], r)) {
        perspectiveCamera.SetUpFrameBuffer(r);
    }

    Space();

    PerspectiveCamera cam = perspectiveCamera.GetData();
    if (PerspectiveCameraWidget(cam)) {
        perspectiveCamera.SetData(std::move(cam));
    }
}

void UserDefinedComponentStorageUI::RenderComponentInstance(Assets& assets, const ComponentInstance& componentInstance) {
    ComponentInstance& instance = const_cast<ComponentInstance&>(componentInstance);
    const auto& component{ assets.FindAsset(instance.ComponentAssetID())->DataAs<Component>() };
    for (int i = 0; i < component.fields.size(); i++) {
        auto& field{ component.fields[i] };
        const auto& type{ field.typeName };
        auto& value{ instance.MemberValues()[i] };
        if (type == "bool") {
            BoolWidget(field.name, value.As<bool&>());
        }
        if (type == "int8") {
            Int8Widget(field.name, value.As<int8_t&>());
        }
        if (type == "int16") {
            Int16Widget(field.name, value.As<int16_t&>());
        }
        if (type == "int") {
            Int32Widget(field.name, value.As<int32_t&>());
        }
        if (type == "long") {
            Int64Widget(field.name, value.As<int64_t&>());
        }
        if (type == "uint8") {
            UInt8Widget(field.name, value.As<uint8_t&>());
        }
        if (type == "uint16") {
            UInt16Widget(field.name, value.As<uint16_t&>());
        }
        if (type == "unsigned int") {
            UInt32Widget(field.name, value.As<uint32_t&>());
        }
        if (type == "unsigned long") {
            UInt64Widget(field.name, value.As<uint64_t&>());
        }
        if (type == "float") {
            FloatWidget(field.name, value.As<float_t&>());
        }
        if (type == "double") {
            DoubleWidget(field.name, value.As<double_t&>());
        }
    }
}

bool ComponentUI::Begin(const Observer& observer, std::string_view componentTypeName) {
    std::string title(Prettify(std::string(componentTypeName)));
    std::string titleWithIcon;
    auto icon = ComponentIcons::DEFINED_COMPONENT_ICONS.find(std::string(componentTypeName));
    if (icon != ComponentIcons::DEFINED_COMPONENT_ICONS.end()) {
        titleWithIcon = icon->second + " " + title;
    } else {
        titleWithIcon = ComponentIcons::GENERIC_COMPONENT_ICON + title;
    }
    ImGui::PushFont(observer.gui.get().GetFontBold());

    bool rv = ImGui::CollapsingHeader(titleWithIcon.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
    if (rv) {
        ImGui::PushFont(observer.gui.get().GetFont());
    }
    return rv;
}
void ComponentUI::RenderIDComponent(const Observer& observer, const IDComponent& idComponent) {
    bool show = ComponentUI::Begin(observer, nameof(IDComponent));
    if (show) {
        IDComponentUI::Render(idComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderTransformComponent(const Observer& observer, const TransformComponent& transformComponent) {
    bool show = ComponentUI::Begin(observer, nameof(TransformComponent));
    if (show) {
        TransformComponentUI::Render(transformComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderParentComponent(const Observer& observer, const ParentComponent& parentComponent) {
    bool show = ComponentUI::Begin(observer, nameof(ParentComponent));
    if (show) {
        ParentComponentUI::Render(parentComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderChildComponent(const Observer& observer, const ChildComponent& childComponent) {
    bool show = ComponentUI::Begin(observer, nameof(ChildComponent));
    if (show) {
        ChildComponentUI::Render(childComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderOrthoCameraComponent(const Observer& observer, const OrthoCameraComponent& orthoCameraComponent) {
    bool show = ComponentUI::Begin(observer, nameof(OrthoCameraComponent));
    if (show) {
        OrthoCameraComponentUI::Render(orthoCameraComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderPerspectiveCameraComponent(const Observer& observer, const PerspectiveCameraComponent& perspectiveCameraComponent) {
    bool show = ComponentUI::Begin(observer, nameof(PerspectiveCameraComponent));
    if (show) {
        PerspectiveCameraComponentUI::Render(perspectiveCameraComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderUserDefinedComponentStorage(const Observer& observer, const UserDefinedComponentStorage& storageComponent) {
    for (auto& [name, instance] : storageComponent.Components()) {
        bool show = ComponentUI::Begin(observer, name);
        if (show) {
            Assets& assets{ observer.gui.get().openProject->Assets() };
            UserDefinedComponentStorageUI::RenderComponentInstance(assets, instance);
        }
        ComponentUI::End(show);
    }
}
void ComponentUI::End(bool show) {
    if (show) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
        ImGui::PopFont();
    }
    ImGui::PopFont();
}