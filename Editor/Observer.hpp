#pragma once

#include <string_view>
#include <unordered_map>
#include <functional>
#include <optional>

#include "Entity.hpp"

struct GUI;
struct Scene;

struct Observer {

	std::reference_wrapper<GUI> gui;

	Observer(GUI& gui) noexcept;

	void Begin(Scene* scene);
	void Render(Scene& scene);
	void End();

	void RenderComponents(Scene& scene, const Entity entt);

};
