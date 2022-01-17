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

	Renderer() noexcept;
	~Renderer() noexcept;
	Renderer(const Renderer&) = delete;
	Renderer(const Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(const Renderer&&) = delete;

	void Register(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model);
	void Submit(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model, EntityID instance);

	void Render(entt::registry& registry, ACamera* cam);
};
