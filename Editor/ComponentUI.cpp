#include <Editor/ComponentUI.hpp>

#include <unordered_map>

#include <Utility/NameOf.hpp>
#include <Utility/Prettify.hpp>
#include <Utility/ConstexprConcat.hpp>

#include <Engine/Scene.hpp>
#include <Engine/IDComponent.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/MultiMaterialComponent.hpp>
#include <Engine/CameraComponent.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Observer.hpp>
#include <Editor/AssetFilter.hpp>
#include <Editor/ComponentWidgets.hpp>
#include <Editor/UserDefinedComponentStorage.hpp>

#include <Editor/RenameEntityCommand.hpp>
#include <Editor/TranslateEntityCommand.hpp>
#include <Editor/RotateEntityCommand.hpp>
#include <Editor/ScaleEntityCommand.hpp>

void IDComponentUI::Render(GUI& gui, const IDComponent& idComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(IDComponent::entity), Prettify(nameof(IDComponent::entity)) },
        { nameof(IDComponent::tag), Prettify(nameof(IDComponent::tag)) }
    };

    UneditableEntityWidget(UINames[nameof_c(IDComponent::entity)], idComponent.GetEntity());
    std::string newName = std::string(idComponent.GetTag());
    if (StringWidget(UINames[nameof_c(IDComponent::tag)], newName)) {
        gui.ExecuteCommand<RenameEntityCommand>(idComponent.GetEntity(), newName);
    }
}

void TransformComponentUI::Render(GUI& gui, const TransformComponent& transformComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(TransformComponent::localTranslation), Prettify(nameof(TransformComponent::localTranslation)) },
        { nameof(TransformComponent::localRotation), Prettify(nameof(TransformComponent::localRotation)) },
        { nameof(TransformComponent::localScale), Prettify(nameof(TransformComponent::localScale)) }
    };

    // translation
    {
        glm::vec3 translation = transformComponent.GetLocalTranslation();
        if (FancyVector3Widget(UINames[nameof(TransformComponent::localTranslation)], translation)) {
            gui.ExecuteCommand<TranslateEntityCommand>(transformComponent.GetEntity(), translation);
        }
    }
    // rotation
    {
        glm::quat quat = transformComponent.GetLocalRotation();
        glm::vec3 eulersDeg(glm::degrees(glm::eulerAngles(quat)));
        if (FancyVector3Widget(UINames[nameof(TransformComponent::localRotation)], eulersDeg)) {
            gui.ExecuteCommand<RotateEntityCommand>(transformComponent.GetEntity(), glm::quat(glm::radians(eulersDeg)));
        }
    }
    // scale
    {
        glm::vec3 scale = transformComponent.GetLocalScale();
        FancyVectorWidgetSettings<Display::XYZ> settings;
        settings.resetTo = 1.0f;
        if (FancyVector3Widget(UINames[nameof(TransformComponent::localScale)], scale, settings)) {
            gui.ExecuteCommand<ScaleEntityCommand>(transformComponent.GetEntity(), scale);
        }
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

    [[maybe_unused]] ChildComponent& child = const_cast<ChildComponent&>(childComponent);
    UneditableEntityWidget(UINames[nameof(ChildComponent::parent)], childComponent.GetParent());
}

void MultiMaterialComponentUI::Render(GUI& gui, const MultiMaterialComponent& multiMaterialComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(MultiMaterialComponent::materials), Prettify(nameof(MultiMaterialComponent::materials)) }
    };

    MultiMaterialComponent& mmc = const_cast<MultiMaterialComponent&>(multiMaterialComponent);
    MultiAssetWidget(UINames[nameof(MultiMaterialComponent::materials)], mmc.GetMaterials(), *Core::GetCore()->GetAssets().get(), gui.GetMetaAssetInfoBank(), AssetFilters::IncludeMaterialAssets() | AssetFilters::IncludeTextureAssets());
}

void OrthoCameraComponentUI::Render(const OrthoCameraComponent& orthoCameraComponent) {
    static unordered_string_map<std::string> UINames = {
        { nameof(OrthoCameraComponent::isActiveAndRendering), Prettify(nameof(OrthoCameraComponent::isActiveAndRendering)) },
        { nameof(OrthoCameraComponent::data), Prettify("orthoCameraProperties") },
        { nameof(ClearColor), Prettify("clearColor") },
        { nameof(OrthoCameraComponent::frameBuffer), Prettify("resolution") }
    };

    OrthoCameraComponent& orthoCamera = const_cast<OrthoCameraComponent&>(orthoCameraComponent);
    bool active = orthoCamera.IsActiveAndRendering();
    if (BoolWidget(UINames[nameof(OrthoCameraComponent::isActiveAndRendering)], active)) {
        if (active) { orthoCamera.TurnOn(); } else { orthoCamera.TurnOff(); }
    }

    Space();
    Header(UINames[nameof(OrthoCameraComponent::data)]);

    //ColorWidget(UINames[nameof(FrameBuffer::ClearColor)], orthoCamera.ClearColor);

    //Resolution r = orthoCamera.GetFrameBuffer();
    //if (ResolutionWidget(UINames[nameof(OrthoCameraComponent::frameBuffer)], r)) {
    //   orthoCamera.SetUpFrameBuffer(r);
    //}

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
        { nameof(ClearColor), Prettify("clearColor") },
        { nameof(PerspectiveCameraComponent::frameBuffer), Prettify("resolution") }
    };

    PerspectiveCameraComponent& perspectiveCamera = const_cast<PerspectiveCameraComponent&>(perspectiveCameraComponent);
    bool active = perspectiveCamera.IsActiveAndRendering();
    if (BoolWidget(UINames[nameof(PerspectiveCameraComponent::isActiveAndRendering)], active)) {
        if (active) { perspectiveCamera.TurnOn(); } else { perspectiveCamera.TurnOff(); }
    }

    Space();
    Header(UINames[nameof(PerspectiveCameraComponent::data)]);

    //ColorWidget(UINames[nameof(PerspectiveCameraComponent::ClearColor)], perspectiveCamera.ClearColor);

    //Resolution r = perspectiveCamera.GetFrameBuffer()._resolution;
    //if (ResolutionWidget(UINames[nameof(PerspectiveCameraComponent::frameBuffer)], r)) {
    //    perspectiveCamera.SetUpFrameBuffer(r);
    //}

    Space();

    PerspectiveCamera cam = perspectiveCamera.GetData();
    if (PerspectiveCameraWidget(cam)) {
        perspectiveCamera.SetData(std::move(cam));
    }
}

void UserDefinedComponentStorageUI::RenderComponentInstance(const ComponentInstance& componentInstance) {
    ComponentInstance& instance = const_cast<ComponentInstance&>(componentInstance);
    AssetHandle cmpAsset{ Core::GetCore()->GetAssets()->FindAsset(instance.ComponentAssetID()) };
    const auto& component{ cmpAsset->DataAs<Component>() };
    if (!cmpAsset.HasValue() || cmpAsset->HasErrorMessages()) { return; }
    for (size_t i = 0; i < component.fields.size(); i++) {
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
    std::string titleWithIcon = ComponentIcons::FindIconByComponentName(componentTypeName) + title;
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
        IDComponentUI::Render(observer.gui, idComponent);
    }
    ComponentUI::End(show);
}
void ComponentUI::RenderTransformComponent(const Observer& observer, const TransformComponent& transformComponent) {
    bool show = ComponentUI::Begin(observer, nameof(TransformComponent));
    if (show) {
        TransformComponentUI::Render(observer.gui, transformComponent);
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
void ComponentUI::RenderMultiMaterialComponent(const Observer& observer, const MultiMaterialComponent& multiMaterialComponent) {
    bool show = ComponentUI::Begin(observer, nameof(MultiMaterialComponent));
    if (show) {
        MultiMaterialComponentUI::Render(observer.gui, multiMaterialComponent);
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
    /* casting away const is safe, storage is never created const */
    auto& storage{ const_cast<UserDefinedComponentStorage&>(storageComponent) };
    UUID removee{ UUID::Empty() };

    for (auto& [name, instance] : storage.Components()) {
        bool show{ false };
        if (!instance.HasError()) {
            show = ComponentUI::Begin(observer, name);
        } else {
            std::string title(Prettify(name));
            std::string titleWithIcon = ComponentIcons::FindIconForInstantiationError(instance.GetError()) + title;
            ImGui::PushFont(observer.gui.get().GetFontBold());
            ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.5f, 0.1f, 1.0f });
            ImGui::CollapsingHeader(titleWithIcon.c_str(), ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", instance.ErrorString().data());
            }
            ImGui::PopStyleColor();
        }
#pragma region ContextMenu
        ImGui::PushFont(observer.gui.get().GetFontBold());
        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem(cat(ObserverIcons::ContextMenu::RESET_COMPONENT_DATA_ICON, "Reset Component Data"))) {
                for (auto& member : instance.MemberValues()) {
                    member.Reset();
                }
            }
            if (ImGui::MenuItem(cat(ObserverIcons::ContextMenu::DETACH_COMPONENT_ICON, "Detach Component"))) {
                removee = instance.ComponentAssetID();
            }
            ImGui::EndPopup();
        }
        ImGui::PopFont();
#pragma endregion
        if (show) {
            UserDefinedComponentStorageUI::RenderComponentInstance(instance);
        }
        ComponentUI::End(show);
    }

    if (removee != UUID::Empty()) {
        storage.DetachComponent(removee);
    }
}
void ComponentUI::End(bool show) {
    if (show) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
        ImGui::PopFont();
    }
    ImGui::PopFont();
}
