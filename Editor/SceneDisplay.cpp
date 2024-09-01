#include <Editor/SceneDisplay.hpp>

#include <Utility/NameOf.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Observer.hpp>
#include <Editor/UserDefinedComponentStorage.hpp>

#include <Engine/IDComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/CameraComponent.hpp>

SceneDisplay::SceneDisplay(Observer& observer) noexcept :
    observer(observer) {}

void SceneDisplay::RenderEntities(const Scene& scene) const noexcept {
    GUI& gui = observer.get().gui;

    auto flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("entities", 1, flags)) {
        ImGui::TableSetupColumn("Entities", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (Entity entt : scene.GetAllEntites()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            std::string label = std::to_string(entt).append("\t\t(").append(scene.GetComponent<IDComponent>(entt).GetTag()).append(")");
            if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth)) {
                std::string nameWithIcon{};

                nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(IDComponent)) + nameof(IDComponent);
                ImGui::TextUnformatted(nameWithIcon.c_str());

                nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(TransformComponent)) + nameof(TransformComponent);
                ImGui::TextUnformatted(nameWithIcon.c_str());
                if (scene.HasComponent<ParentComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(ParentComponent)) + nameof(ParentComponent);
                    ImGui::TextUnformatted(nameWithIcon.c_str());
                }
                if (scene.HasComponent<ChildComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(ChildComponent)) + nameof(ChildComponent);
                    ImGui::TextUnformatted(nameWithIcon.c_str());
                }
                if (scene.HasComponent<OrthoCameraComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(OrthoCameraComponent)) + nameof(OrthoCameraComponent);
                    ImGui::TextUnformatted(nameWithIcon.c_str());
                }
                if (scene.HasComponent<PerspectiveCameraComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(PerspectiveCameraComponent)) + nameof(PerspectiveCameraComponent);
                    ImGui::TextUnformatted(nameWithIcon.c_str());
                }
                if (scene.HasComponent<UserDefinedComponentStorage>(entt)) {
                    auto& udcs{ scene.GetComponent<UserDefinedComponentStorage>(entt) };
                    for (const auto& [name, instance] : udcs.Components()) {
                        if (!instance.HasError()) {
                            AssetHandle cmpAsset{ gui.CORE->GetAssets()->FindAsset(instance.ComponentAssetID()) };
                            nameWithIcon = ComponentIcons::FindIconByComponentName(name) + name;
                            ImGui::TextUnformatted(nameWithIcon.c_str());
                        } else {
                            ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.5f, 0.1f, 1.0f });

                            nameWithIcon = ComponentIcons::FindIconForInstantiationError(instance.GetError()) + name;
                            ImGui::TextUnformatted(nameWithIcon.c_str());
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("%s", instance.ErrorString().data());
                            }

                            ImGui::PopStyleColor();
                        }
                    }
                }
            }
        }
        ImGui::EndTable();
    }
}

void SceneDisplay::RenderSystems([[maybe_unused]] const Scene& scene) const noexcept {
    auto flags = ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_NoBordersInBodyUntilResize;
    if (ImGui::BeginTable("systems", 2, flags)) {
        ImGui::TableSetupColumn("Systems", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize);
        ImGui::TableHeadersRow();

        ImGui::EndTable();
    }
}
