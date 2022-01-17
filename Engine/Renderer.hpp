#pragma once

#include <tuple>
#include <memory>
#include <unordered_map>

#include <entt.hpp>

#include "Entity.hpp"
#include "TypedefsAndConstants.hpp"

struct Model;
struct Shader;
struct ACamera;

struct Renderer {

	std::unordered_map<std::shared_ptr<Shader>, std::vector<std::tuple<std::weak_ptr<Model>, std::vector<Entity>, int, VBO>>> _previous;
	std::unordered_map<std::shared_ptr<Shader>, std::vector<std::tuple<std::weak_ptr<Model>, std::vector<Entity>, int, VBO>>> _current;

	int vertices = 0;
	int indices = 0;
	int drawCalls = 0;

	void Register(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model);
	void Submit(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model, Entity instance);

	void Render(entt::registry& registry, ACamera* cam);
};
