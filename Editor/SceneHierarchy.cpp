#include "SceneHierarchy.hpp"

#include <map>

#include <imgui.h>

#include <EZEasing.hpp>
#include <Utility/ConstexprConcat.hpp>

#include <Engine/Scene.hpp>
#include <Engine/IDComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/CameraComponent.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/UserDefinedComponentStorage.hpp>

SceneHierarchy::SceneHierarchy(GUI& gui) noexcept :
    gui(gui) {
    gui.Events.SceneHierarchy.OnEntitySelected   += std::bind_front(&SceneHierarchy::OnEntitySelected  , this);
    gui.Events.SceneHierarchy.OnEntityDeselected += std::bind_front(&SceneHierarchy::OnEntityDeselected, this);
    gui.Events.OnEntityDeleted                   += std::bind_front(&SceneHierarchy::OnEntityDeleted   , this);
}

bool SceneHierarchy::Begin() {
    ImGui::PushID(GUI::SCENE_HIERARCHY_TITLE);
    std::string title(WindowIcons::SCENE_HIERARCHY_WINDOW_ICON);
    title.append(GUI::SCENE_HIERARCHY_TITLE);
    title.append(GUI::SCENE_HIERARCHY_ID);
    bool visible = ImGui::Begin(title.c_str());

    return visible;
}

void SceneHierarchy::Render() {
    GUI& gui = this->gui;
    if (!gui.HasOpenScene()) { return; }
    Scene& scene = gui.GetOpenScene();

    if (ImGui::BeginDragDropTarget()) {
        auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
        if (payload != nullptr) {
            // dropped an entity into scene hieratchy panel, dropped should be adopted by NULL_ENTT
            Entity dropped = *((Entity*) payload->Data);
            ChildComponent& child = scene.GetComponent<ChildComponent>(dropped);
            ParentComponent& parent = scene.GetComponent<ParentComponent>(child.GetParent());

            child.SetParent(NULL_ENTT);
            scene.RemoveComponent<ChildComponent>(child.GetEntity());

            std::erase(parent.GetChildren(), dropped);
            if (parent.GetChildren().empty()) {
                scene.RemoveComponent<ParentComponent>(parent.GetEntity());
            }

            ImGui::EndDragDropTarget();
        }
    }

    std::string title(SceneHierarchyIcons::SCENE_ICON);
    title.reserve(64);
    title.append(scene.Name);
    if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {

        if (ImGui::BeginDragDropTarget()) {
            auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
            if (payload != nullptr) {
                // dropped an entity onto scene heirarchy panel title
                Entity dropped = *((Entity*) payload->Data);
                ChildComponent& child = scene.GetComponent<ChildComponent>(dropped);
                ParentComponent& parent = scene.GetComponent<ParentComponent>(child.GetParent());

                child.SetParent(NULL_ENTT);
                scene.RemoveComponent<ChildComponent>(child.GetEntity());

                std::erase(parent.GetChildren(), dropped);
                if (parent.GetChildren().empty()) {
                    scene.RemoveComponent<ParentComponent>(parent.GetEntity());
                }

                ImGui::EndDragDropTarget();
            }
        }

        // render entities with no parent (parent == NULL_ENTT);
        auto view = scene.GetRegistry().view<IDComponent>(entt::exclude<ChildComponent>);
        for (auto& entt : view) {
            RenderEntityNode(entt);
        }
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
        gui.Events.SceneHierarchy.OnEntityDeselected();
    }

    if (ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem(cat(SceneHierarchyIcons::ContextMenu::CREATE_NEW_ENTITY_ICON, "Create New Entity"))) {
            gui.Events.OnEntityCreated(scene.CreateEntity());
        }
        ImGui::Separator();
        if (ImGui::MenuItem(cat(SceneHierarchyIcons::ContextMenu::CLOSE_SCENE_ICON, "Close Scene"))) {
            gui.CORE->LoadedProject()->CloseScene();
            gui.Events.OnSceneClosed();
        }
        ImGui::EndPopup();
    }

    if (deletedEntity != NULL_ENTT) {
        scene.DeleteEntity(deletedEntity);
        gui.Events.OnEntityDeleted(deletedEntity);
    }
}

void SceneHierarchy::End() {
    ImGui::End();
    ImGui::PopID();
}

void SceneHierarchy::RenderEntityNode(const Entity entity) {
    GUI& gui = this->gui;
    if (!gui.HasOpenScene()) { return; }
    Scene& scene = gui.GetOpenScene();

    const IDComponent& id = scene.GetComponent<IDComponent>(entity);

    std::string title;
    title.reserve(64);
    title.append(SceneHierarchyIcons::ENTITY_ICON).append(id.GetTag()).append("###Entity").append(std::to_string(EntityTo<uint32_t>(id.GetEntity())));

    bool isLeaf = true;
    if (scene.HasComponent<ParentComponent>(entity)) {
        ParentComponent& parent = scene.GetComponent<ParentComponent>(entity);
        isLeaf = parent.GetChildren().empty();
    }

    panel_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    if (!isLeaf) {
        panel_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    } else {
        panel_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    if (entity == selectedEntity) {
        panel_flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (entity == highlightedEntity) {
        panel_flags |= ImGuiTreeNodeFlags_Selected;
        static auto& origHeaderColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        static auto& origTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        float factor = _highlightTime / highlight_expire;
        ImGui::PushStyleColor(ImGuiCol_Header, {
            eze::easeInSine(1.0f, origHeaderColor.x, factor),
            eze::easeInSine(1.0f, origHeaderColor.y, factor),
            eze::easeInSine(0.31f, origHeaderColor.z, factor),
            1
        });
        ImGui::PushStyleColor(ImGuiCol_Text, {
            eze::easeInSine(0.0f, origTextColor.x, factor),
            eze::easeInSine(0.0f, origTextColor.y, factor),
            eze::easeInSine(0.0f, origTextColor.z, factor),
            1
        });
        _highlightTime += gui.delta;

        // if highlighted entity changed during the highlight anim., restart the anim
        if (lastHighlighted != highlightedEntity) {
            _highlightTime = 0;
        }
        lastHighlighted = highlightedEntity;
    }

    bool opened = ImGui::TreeNodeEx(title.c_str(), panel_flags);
    if (entity == highlightedEntity) {
        ImGui::PopStyleColor(2);
        if (_highlightTime > highlight_expire) {
            _highlightTime = 0;
            highlightedEntity = NULL_ENTT;
        }
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("SELECTED_ENTT", &entity, sizeof(Entity));
        std::string txt;
        txt.append("DragDrop - ").append(id.GetTag());
        ImGui::Text(txt.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
        if (payload != nullptr) {
            // entity dropped is dropped onto current,
            // current should adopt dropped iff:
            // 1. current is not dropped
            // 2. current is not a descendant of dropped
            bool badAdopt{ false };
            const Entity dropped = *(Entity*) payload->Data;
            Entity current = entity;

            while (scene.HasComponent<ChildComponent>(current)) {
                const auto& child = scene.GetComponent<ChildComponent>(current);
                if (child.GetEntity() == dropped) {
                    badAdopt = true; break;
                }

                // if has parent,
                if (scene.GetComponent<ChildComponent>(current).GetParent() != NULL_ENTT) {
                    // check parent
                    current = child.GetParent();
                } else {
                    break;
                }
            }

            if (!badAdopt) {
                if (!scene.HasComponent<ChildComponent>(dropped)) {
                    scene.EmplaceComponent<ChildComponent>(dropped);
                }
                if (!scene.HasComponent<ParentComponent>(entity)) {
                    scene.EmplaceComponent<ParentComponent>(entity);
                }
                ChildComponent& child = scene.GetComponent<ChildComponent>(dropped);
                ParentComponent& parent = scene.GetComponent<ParentComponent>(entity);

                if (child.GetParent() != NULL_ENTT) {
                    std::erase(scene.GetComponent<ParentComponent>(child.GetParent()).GetChildren(), child.GetEntity());
                }
                child.SetParent(parent.GetEntity());
                parent.GetChildren().push_back(child.GetEntity());
            }
            ImGui::EndDragDropTarget();
        }
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        gui.Events.SceneHierarchy.OnEntitySelected(entity);
    }
    RenderContextMenu(entity);

    if (opened) {
        if (scene.HasComponent<ParentComponent>(entity)) {
            ParentComponent& parent = scene.GetComponent<ParentComponent>(entity);
            const std::vector<Entity>& children = parent.GetChildren();

            for (auto& entt : children) {
                RenderEntityNode(entt);
            }
        }

        if (!isLeaf) {
            ImGui::TreePop();
        }
    }
}
void SceneHierarchy::RenderContextMenu(const Entity entity) {
    GUI& gui = this->gui;
    if (!gui.HasOpenScene()) { return; }
    Scene& scene = gui.GetOpenScene();

    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::BeginMenu(cat(SceneHierarchyIcons::ContextMenu::ATTACH_COMPONENT_ICON, "Attach Component"))) {
            // cpp components start
            if (ImGui::MenuItem("Ortho")) {
                scene.EmplaceComponent<OrthoCameraComponent>(entity);
            }
            if (ImGui::MenuItem("Perspec")) {
                scene.EmplaceComponent<PerspectiveCameraComponent>(entity);
            }
            // cpp components end

            // script components start
            if (!scene.HasComponent<UserDefinedComponentStorage>(entity)) {
                scene.EmplaceComponent<UserDefinedComponentStorage>(entity);
            }
            UserDefinedComponentStorage& storage = scene.GetComponent<UserDefinedComponentStorage>(entity);
            auto& assets{ gui.CORE->Assets() };
            for (auto cmpid : assets->ComponentDefinitionAssetIDs()) {
                AssetHandle handle = assets->FindAsset(cmpid);
                if (!handle->HasDeserializedData()) { continue; }

                const Component& cmp = handle->DataAs<Component>();
                std::string name = cmp.name;
                auto icon = ComponentIcons::DEFINED_COMPONENT_ICONS.find(name);
                if (icon != ComponentIcons::DEFINED_COMPONENT_ICONS.end()) {
                    name.insert(0, icon->second);
                } else {
                    name.insert(0, ComponentIcons::GENERIC_COMPONENT_ICON);
                }
                name.insert(0, " ");
                if (ImGui::MenuItem(name.c_str())) {
                    storage.AttachComponent(cmpid);
                }
            }
            //script components end
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem(cat(SceneHierarchyIcons::ContextMenu::DELETE_ENTITY_ICON, "Delete Entity"))) {
            deletedEntity = entity;
        }
        ImGui::EndPopup();
    }
}

void SceneHierarchy::OnEntitySelected(Entity entity) {
    selectedEntity = entity;
}
void SceneHierarchy::OnEntityDeselected() {
    selectedEntity = NULL_ENTT;
}
void SceneHierarchy::OnEntityDeleted(Entity entity) {
    if (selectedEntity == entity) {
        selectedEntity = NULL_ENTT;
    }
    deletedEntity = NULL_ENTT;
}