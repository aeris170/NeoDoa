#pragma once

#include <string_view>
#include <unordered_map>
#include <functional>
#include <optional>

#include "Entity.hpp"

struct GUI;
struct Scene;

struct Observer {

	GUI* const gui;

	Observer(GUI* gui) noexcept;

	void Begin(const std::optional<Scene>& scene);
	void Render(Scene& scene);
	void End();

	void RenderComponents(Scene& scene, const Entity entt);

};
