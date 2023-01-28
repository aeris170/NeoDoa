#include "SceneHierarchy.hpp"

#include <map>

#include <imgui.h>
#include <EZEasing.hpp>

#include "GUI.hpp"
#include "Icons.hpp"

#include "UserDefinedComponentStorage.hpp"

#include <Scene.hpp>
#include <IDComponent.hpp>
#include <ParentComponent.hpp>
#include <ChildComponent.hpp>
#include <CameraComponent.hpp>

SceneHierarchy::SceneHierarchy(GUI& gui) noexcept :
    gui(gui) {}

void SceneHierarchy::Begin() {
    GUI& gui = this->gui;
    ImGui::PushID(GUI::SCENE_HIERARCHY_TITLE);
    std::string title(WindowIcons::SCENE_HIERARCHY_WINDOW_ICON);
    title.append(GUI::SCENE_HIERARCHY_TITLE);
    title.append(GUI::SCENE_HIERARCHY_ID);
    ImGui::Begin(title.c_str());
}

void SceneHierarchy::Render(Scene& scene) {
    GUI& gui = this->gui;
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
            RenderEntityNode(scene, entt);
        }
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
        ResetSelectedEntity();
    }

    if (ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem(ICON_FA_PLUS " Create New Entity")) {
            scene.CreateEntity();
        }
        ImGui::Separator();
        if (ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Close Scene")) {
            gui.openProject.value().CloseScene();
        }
        ImGui::EndPopup();
    }

    if (deletedEntity != NULL_ENTT) {
        scene.DeleteEntity(deletedEntity);
        if (selectedEntity == deletedEntity) {
            ResetSelectedEntity();
        }
        deletedEntity = NULL_ENTT;
    }
}

void SceneHierarchy::End() {
    ImGui::End();
    ImGui::PopID();
}

void SceneHierarchy::RenderEntityNode(Scene& scene, const Entity entity) {
    GUI& gui = this->gui;
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
                ChildComponent& child = scene.GetComponent<ChildComponent>(current);
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
        SetSelectedEntity(entity);
    }

    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::BeginMenu(ICON_FA_LINK " Attach Module")) {
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
            Assets& assets{ gui.openProject->Assets() };
            for (auto cmpid : assets.ComponentDefinitionAssetIDs()) {
                AssetHandle handle = assets.FindAsset(cmpid);
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
                    storage.AttachComponent(assets, cmpid);
                }
            }
            //script components end
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem(ICON_FA_TRASH " Delete Entity")) {
            deletedEntity = entity;
        }
        ImGui::EndPopup();
    }

    if (opened) {
        if (scene.HasComponent<ParentComponent>(entity)) {
            ParentComponent& parent = scene.GetComponent<ParentComponent>(entity);
            const std::vector<Entity>& children = parent.GetChildren();

            for (auto& entt : children) {
                RenderEntityNode(scene, entt);
            }
        }

        if (!isLeaf) {
            ImGui::TreePop();
        }
    }
}

void SceneHierarchy::SetSelectedEntity(Entity entt) {
    GUI& gui = this->gui;
    selectedEntity = entt;
    gui.obs.SetDisplayTarget(selectedEntity);
}
void SceneHierarchy::ResetSelectedEntity() {
    GUI& gui = this->gui;
    selectedEntity = NULL_ENTT;
    gui.obs.ResetDisplayTarget();
}