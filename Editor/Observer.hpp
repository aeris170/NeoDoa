#pragma once

#include <string_view>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>

#include "Entity.hpp"

#include "TemplateUtilities.hpp"

struct GUI;
struct Scene;
struct FNode;

#define DISPLAYABLE Entity, FNode*
template<typename T>
concept Displayable = concepts::IsAnyOf<T, DISPLAYABLE>;
using DisplayTarget = std::variant<std::monostate, DISPLAYABLE>;
#undef DISPLAYABLE

struct Observer {

	std::reference_wrapper<GUI> gui;

	Observer(GUI& gui) noexcept;

	void Begin(Scene* scene);
	void Render(Scene& scene);
	void End();

	void RenderComponents(Scene& scene, const Entity entt);

	template<Displayable T>
	void SetDisplayTarget(T target) { displayTarget == target; }

private:
	DisplayTarget displayTarget;
};
