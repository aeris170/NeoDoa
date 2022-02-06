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
#include <ScriptStorageComponent.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "ComponentUI.hpp"

Observer::Observer(GUI* gui) noexcept :
	gui(gui){}

void Observer::Begin(const std::optional<Scene>& scene) {
	ImGui::PushID(gui->OBSERVER_TITLE);
	std::string title(WindowIcons::OBSERVER_WINDOW_ICON);
	title.append(gui->OBSERVER_TITLE);
	if(scene) {
		if (gui->sh.selectedEntity != NULL_ENTT) {
			title.append(" - ");
			title.append(scene.value().GetComponent<IDComponent>(gui->sh.selectedEntity).GetTag());
		}
	}
	title.append(gui->OBSERVER_ID);
	ImGui::Begin(title.c_str());
}

void Observer::Render(Scene& scene) {
	if (gui->sh.selectedEntity != NULL_ENTT) {
		RenderComponents(scene, gui->sh.selectedEntity);
	} else {
		const char* text = "Select an entity from the scene :)";
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 txtSize = ImGui::CalcTextSize(text);
		for (int i = 0; i < 5; i++) {
			ImGui::SameLine((size.x - txtSize.x) / 2, 0);
			ImGui::Text(text);
			ImGui::NewLine();
		}
	}
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

	if (scene.HasComponent<ScriptStorageComponent>(entt)) {
		ScriptStorageComponent& scriptStorageComponent = scene.GetComponent<ScriptStorageComponent>(entt);
		ComponentUI::RenderScriptStorageComponent(*this, scene, scriptStorageComponent);
	}
}
