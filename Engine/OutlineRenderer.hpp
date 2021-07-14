#pragma once

#include <glm/glm.hpp>
#include <entt.hpp>

struct Transform;
struct ModelRenderer;
struct Camera;

struct OutlineRenderer {

	int vertices = 0;
	int indices = 0;
	int drawCalls = 0;

	OutlineRenderer() noexcept;
	~OutlineRenderer() noexcept;
	OutlineRenderer(const OutlineRenderer&) = delete;
	OutlineRenderer(const OutlineRenderer&&) = delete;
	OutlineRenderer& operator=(const OutlineRenderer&) = delete;
	OutlineRenderer& operator=(const OutlineRenderer&&) = delete;

	void Render(std::vector<std::tuple<Transform&, ModelRenderer&>>& objects, Camera* cam, glm::vec3 outlineColor);
};
