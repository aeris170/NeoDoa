#include "ComponentUI.hpp"

#include <unordered_map>

#include <nameof.hpp>
#include <prettify.hpp>

#include <IDComponent.hpp>
#include <TransformComponent.hpp>
#include <ParentComponent.hpp>
#include <ChildComponent.hpp>

#include "GUI.hpp"
#include "Scene.hpp"
#include "Observer.hpp"
#include "ComponentWidgets.hpp"

void IDComponentUI::Render(const IDComponent& idComponent) {
	static std::unordered_map<::std::string, ::std::string> UINames = {
		{ nameof(IDComponent::entity), Prettify(nameof(IDComponent::entity)) },
		{ nameof(IDComponent::tag), Prettify(nameof(IDComponent::tag)) }
	};

	IDComponent& id = const_cast<IDComponent&>(idComponent);
	UneditableEntityWidget(UINames[nameof_c(IDComponent::entity)], idComponent.GetEntity());
	StringWidget(UINames[nameof_c(IDComponent::tag)], id.GetTagRef());
}

void TransformComponentUI::Render(const TransformComponent& transformComponent) {
	static std::unordered_map<::std::string, ::std::string> UINames = {
		{ nameof(TransformComponent::localTranslation), Prettify(nameof(TransformComponent::localTranslation)) },
		{ nameof(TransformComponent::localRotation), Prettify(nameof(TransformComponent::localRotation)) },
		{ nameof(TransformComponent::localScale), Prettify(nameof(TransformComponent::localScale)) }
	};

	TransformComponent& transform = const_cast<TransformComponent&>(transformComponent);
	// translation
	{
		glm::vec3 translation = transform.GetLocalTranslation();
		FancyVectorWidget(UINames[nameof_c(TransformComponent::localTranslation)], translation);
		transform.SetLocalTranslation(translation);
	}
	// rotation
	{
		glm::quat quat = transform.GetLocalRotation();
		glm::vec3 eulersDeg(glm::degrees(glm::eulerAngles(quat)));
		glm::vec3 old(eulersDeg);
		FancyVectorWidget(UINames[nameof_c(TransformComponent::localRotation)], eulersDeg);
		quat = quat * glm::quat(glm::radians(eulersDeg - old));
		transform.SetLocalRotation(quat);
	}
	// scale
	{
		glm::vec3 scale = transform.GetLocalScale();
		FancyVectorWidget(UINames[nameof_c(TransformComponent::localScale)], scale, 1.0f);
		transform.SetLocalScale(scale);
	}
}

void ParentComponentUI::Render(const ParentComponent& parentComponent) {
	static std::unordered_map<::std::string, ::std::string> UINames = {
		{ nameof(ParentComponent::children), Prettify(nameof(ParentComponent::children)) }
	};

	ParentComponent& parent = const_cast<ParentComponent&>(parentComponent);
	UneditableArrayWidget<Entity>(UINames[nameof_c(ParentComponent::children)], parent.GetChildren());
}

void ChildComponentUI::Render(const ChildComponent& childComponent) {
	static std::unordered_map<::std::string, ::std::string> UINames = {
		{ nameof(ChildComponent::parent), Prettify(nameof(ChildComponent::parent)) }
	};

	ChildComponent& child = const_cast<ChildComponent&>(childComponent);
	UneditableEntityWidget(UINames[nameof_c(ChildComponent::parent)], childComponent.GetParent());
}

bool ComponentUI::Begin(const Observer& observer, std::string_view componentTypeName) {
	std::string title(Prettify(std::string(componentTypeName)));
	std::string titleWithIcon;
	auto icon = observer.gui->CUSTOM_SCRIPT_ICONS.find(std::string(componentTypeName));
	if (icon != observer.gui->CUSTOM_SCRIPT_ICONS.end()) {
		titleWithIcon = icon->second + " " + title;
	}
	else {
		titleWithIcon = observer.gui->GENERIC_SCRIPT_ICON + title;
	}
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

	bool rv = ImGui::CollapsingHeader(titleWithIcon.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
	if (rv) {
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	}
	return rv;
}
void ComponentUI::RenderIDComponent(const Observer& observer, Scene& scene, IDComponent& idComponent) {
	bool show = ComponentUI::Begin(observer, nameof(IDComponent));
	if (show) {
		IDComponentUI::Render(idComponent);
	}
	ComponentUI::End(show);
}
void ComponentUI::RenderTransformComponent(const Observer& observer, Scene& scene, TransformComponent& transformComponent) {
	bool show = ComponentUI::Begin(observer, nameof(TransformComponent));
	if (show) {
		TransformComponentUI::Render(transformComponent);
	}
	ComponentUI::End(show);
}
void ComponentUI::RenderParentComponent(const Observer& observer, Scene& scene, ParentComponent& parentComponent) {
	bool show = ComponentUI::Begin(observer, nameof(ParentComponent));
	if (show) {
		ParentComponentUI::Render(parentComponent);
	}
	ComponentUI::End(show);
}
void ComponentUI::RenderChildComponent(const Observer& observer, Scene& scene, ChildComponent& childComponent) {
	bool show = ComponentUI::Begin(observer, nameof(ChildComponent));
	if (show) {
		ChildComponentUI::Render(childComponent);
	}
	ComponentUI::End(show);
}
void ComponentUI::RenderScriptStorageComponent(const Observer& observer, Scene& scene, ScriptStorageComponent& scriptStorageComponent) {

}
void ComponentUI::End(bool show) {
	if (show) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::PopFont();
	}
	ImGui::PopFont();
}