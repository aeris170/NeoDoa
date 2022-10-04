#include "Observer.hpp"

#include <string>
#include <sstream>

#include <imgui.h>
#include <nameof.hpp>
#include <prettify.hpp>

#include <Scene.hpp>
#include <IDComponent.hpp>
#include <TransformComponent.hpp>
#include <ParentComponent.hpp>
#include <ChildComponent.hpp>
#include <CameraComponent.hpp>
#include <ScriptStorageComponent.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "ComponentUI.hpp"

Observer::Observer(GUI& gui) noexcept :
	gui(gui){}

void Observer::Begin(Scene* scene) {
	GUI& gui = this->gui;
	ImGui::PushID(gui.OBSERVER_TITLE);
	std::string title(WindowIcons::OBSERVER_WINDOW_ICON);
	title.append(gui.OBSERVER_TITLE);
	if(scene) {
		if (gui.sh.selectedEntity != NULL_ENTT) {
			title.append(" - ");
			title.append(scene->GetComponent<IDComponent>(gui.sh.selectedEntity).GetTag());
		}
	}
	title.append(gui.OBSERVER_ID);
	ImGui::Begin(title.c_str());
}

void Observer::Render(Scene& scene) {
	GUI& gui = this->gui;
	std::visit(overloaded::lambda {
		[](std::monostate arg) {
			const char* text = "Nothing to display here :)";
			ImVec2 size = ImGui::GetContentRegionAvail();
			ImVec2 txtSize = ImGui::CalcTextSize(text);
			for (int i = 0; i < 5; i++) {
				ImGui::SameLine((size.x - txtSize.x) / 2, 0);
				ImGui::Text(text);
				ImGui::NewLine();
			}
		},
		[&](Entity entt) { RenderComponents(scene, gui.sh.selectedEntity); },
		[&](FNode* file) {
			static const ImVec2 iconSize{ 64.0f, 64.0f };

			if (file->IsDirectory()) {
				ImGui::Columns(2, nullptr, 0);
				ImGui::SetColumnWidth(0, iconSize.x * 1.25f);

				ImGui::PushFont(gui.GetFontBold());

				ImGui::Image(gui.GetFolderIcon(), iconSize);
				ImGui::SameLine();
				auto name = static_cast<const FNode*>(file)->Name().c_str();

				ImGui::NextColumn();

				ImGui::Text(name);

				ImGui::PopFont();

				ImGui::PushFont(gui.GetFont());
				std::string path = file->Path().string();
				ImGui::Text(std::string("Path: ROOT").append(sizeof(char), static_cast<char>(std::filesystem::path::preferred_separator)).append(path).c_str());

				std::string absolutePath = file->AbsolutePath().string();
				ImGui::Text(std::string("Absolute Path: ").append(absolutePath).c_str());
				ImGui::PopFont();

			} else {
				AssetHandle h = gui.openProject->Assets().FindAssetAt(*file);
			}
		}
	}, displayTarget);
}

void Observer::End() {
	ImGui::End();
	ImGui::PopID();
}

void Observer::RenderComponents(Scene& scene, const Entity entt) {
	IDComponent& idComponent = scene.GetComponent<IDComponent>(entt);
	ComponentUI::RenderIDComponent(*this, scene, idComponent);

	TransformComponent& transformComponent = scene.GetComponent<TransformComponent>(entt);
	ComponentUI::RenderTransformComponent(*this, scene, transformComponent);

	if(scene.HasComponent<ParentComponent>(entt)) {
		ParentComponent& parentComponent = scene.GetComponent<ParentComponent>(entt);
		ComponentUI::RenderParentComponent(*this, scene, parentComponent);
	}

	if (scene.HasComponent<ChildComponent>(entt)) {
		ChildComponent& childComponent = scene.GetComponent<ChildComponent>(entt);
		ComponentUI::RenderChildComponent(*this, scene, childComponent);
	}

	if (scene.HasComponent<OrthoCameraComponent>(entt)) {
		OrthoCameraComponent& orthoCameraComponent = scene.GetComponent<OrthoCameraComponent>(entt);
		ComponentUI::RenderOrthoCameraComponent(*this, scene, orthoCameraComponent);
	}

	if (scene.HasComponent<PerspectiveCameraComponent>(entt)) {
		PerspectiveCameraComponent& perspectiveCameraComponent = scene.GetComponent<PerspectiveCameraComponent>(entt);
		ComponentUI::RenderPerspectiveCameraComponent(*this, scene, perspectiveCameraComponent);
	}

	if (scene.HasComponent<ScriptStorageComponent>(entt)) {
		ScriptStorageComponent& scriptStorageComponent = scene.GetComponent<ScriptStorageComponent>(entt);
		ComponentUI::RenderScriptStorageComponent(*this, scene, scriptStorageComponent);
	}
}

void Observer::ResetDisplayTarget() { displayTarget = std::monostate{}; }
